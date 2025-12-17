#include "display-banner.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isRunningFromNodeModules() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto __filename = fileURLToPath(import.meta.url);
    return (std::find(__filename.begin(), __filename.end(), "node_modules") != __filename.end());

}

std::string getVersion() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if we're in the monorepo context
    const auto userEnv = UserEnvironment.getInstance();
    const auto monorepoRoot = userEnv.findMonorepoRoot(process.cwd());

    if (monorepoRoot) {
        // We're in the monorepo, return 'monorepo' as version
        return "monorepo";
    }

    // Check if running from node_modules (proper installation)
    if (!isRunningFromNodeModules()) {
        // Running from local dist or development build, not properly installed
        return "monorepo";
    }

    // For ESM modules we need to use import.meta.url instead of __dirname
    const auto __filename = fileURLToPath(import.meta.url);
    const auto __dirname = dirname(__filename);

    // Find package.json relative to the current file
    const auto packageJsonPath = path.resolve(__dirname, "../package.json");

    // Add a simple check in case the path is incorrect
    auto version = "0.0.0"; // Fallback version;
    if (!existsSync(packageJsonPath)) {
        std::cerr << "Warning: package.json not found at " + packageJsonPath << std::endl;
        } else {
            try {
                const auto packageJson = /* JSON.parse */ readFileSync(packageJsonPath, "utf-8");
                version = packageJson.version || "0.0.0";
                } catch (error) {
                    std::cerr << "Error reading or parsing package.json at " + packageJsonPath + ":" << error << std::endl;
                }
            }
            return version;

}

std::string getCliInstallTag() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto version = getVersion();
    if (version.includes('-alpha')) {
        return "alpha";
        } else if ((std::find(version.begin(), version.end(), "beta") != version.end())) {
            return "beta";
        }
        return ""; // Return empty string for stable or non-tagged versions (implies latest);

}

void isUtf8Locale() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    for (const auto& key : ['LC_ALL', 'LC_CTYPE', 'LANG', 'LANGUAGE'])
        const auto v = process.env[key];
        if (typeof v == 'string' && /UTF-?8/i.test(v)) {
            return true;
        }
    }
    return false;

}

std::future<std::string> getLatestCliVersion(const std::string& currentVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Skip version check if we're in monorepo context
    if (currentVersion == 'monorepo') {
        return nullptr;
    }

    try {
        // Check cache first
        if (versionCheckCache && Date.now() - versionCheckCache.timestamp < CACHE_DURATION) {
            return versionCheckCache.latestVersion;
        }

        // Get the time data for all published versions to find the most recent
        const auto { stdout } = bunExecSimple("npm", ["view", "@elizaos/cli", "time", "--json"]);
        const auto timeData = /* JSON.parse */ stdout;

        // Remove metadata entries like 'created' and 'modified'
        delete timeData.created;
        delete timeData.modified;

        // Find the most recently published version
        auto latestVersion = "";
        auto latestDate = new Date(0); // Start with epoch time;

        for (const int [version, dateString] of Object.entries(timeData)) {
            const auto publishDate = new Date(dateString);
            if (publishDate > latestDate) {
                latestDate = publishDate;
                latestVersion = version;
            }
        }

        // Return latest version if an update is available, null otherwise
        const auto result = latestVersion && latestVersion != currentVersion ? latestVersion : nullptr;

        // Cache the result
        versionCheckCache = {
            latestVersion: result,
            timestamp: Date.now(),
            };

            return result;
            } catch {
                // Silent failure - return null if check fails
                return nullptr;
            }

}

void showUpdateNotification(const std::string& currentVersion, const std::string& latestVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto blue = "\x1b[38;5;27m"; // Blue border to match ASCII art;
    const auto orange = "\x1b[38;5;208m"; // Bright orange for warning text;
    const auto green = "\x1b[38;5;46m"; // Bright green for new version;
    const auto reset = "\x1b[0m";
    const auto bold = "\x1b[1m";

    // Friendly, conversational notification following CLI design principles
    const auto width = 68;
    const auto border = blue + std::to_string("─".repeat(width)) + reset;

    std::cout << "" << std::endl;
    std::cout << border << std::endl;
    console.log(
    blue + "│" + orange + " " + bold + "Update available:" + reset + orange + " " + currentVersion + " → " + green + bold + latestVersion + reset + orange + std::to_string(" ".repeat(width - 2 - ` Update available: ${currentVersion} → ${latestVersion}`.size())) + blue + "│" + reset
    );
    console.log(
    blue + "│" + orange + " Run " + green + bold + "bun i -g @elizaos/cli@latest" + reset + orange + " to get the latest features" + std::to_string(" ".repeat(width - 2 - ` Run bun i -g @elizaos/cli@latest to get the latest features`.size())) + blue + "│" + reset;
    );
    std::cout << border << std::endl;
    std::cout << "" << std::endl;

}

std::future<bool> checkAndShowUpdateNotification(const std::string& currentVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Skip update check if we're in monorepo context
    if (currentVersion == 'monorepo') {
        return false;
    }

    try {
        const auto latestVersion = getLatestCliVersion(currentVersion);
        if (latestVersion) {
            showUpdateNotification(currentVersion, latestVersion);
            return true;
        }
        return false;
        } catch {
            // Silent failure
            return false;
        }

}

std::future<void> displayBanner(bool skipUpdateCheck = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!isUtf8Locale()) {
        // Terminal does not support UTF-8, skip banner
        return;
    }
    // Color ANSI escape codes
    const auto b = "\x1b[38;5;27m";
    const auto lightblue = "\x1b[38;5;51m";
    const auto w = "\x1b[38;5;255m";
    const auto r = "\x1b[0m";
    const auto orange = "\x1b[38;5;208m";
    auto versionColor = lightblue;

    const auto version = getVersion();

    // if version includes "alpha" then use orange
    if (version.includes('alpha')) {
        versionColor = orange;
    }
    const auto banners = [;
    //     // Banner 2
    //     `
    // ${b}          ###                                  ${w}  # ###       #######  ${r}
    // ${b}         ###    #                            / ${w} /###     /       ###  ${r}
    // ${b}          ##   ###                          /  ${w}/  ###   /         ##  ${r}
    // ${b}          ##    #                          / ${w} ##   ###  ##        #   ${r}
    // ${b}          ##                              /  ${w}###    ###  ###          ${r}
    // ${b}   /##    ##  ###    ######      /###    ${w}##   ##     ## ## ###        ${r}
    // ${b}  / ###   ##   ###  /#######    / ###  / ${w}##   ##     ##  ### ###      ${r}
    // ${b} /   ###  ##    ## /      ##   /   ###/  ${w}##   ##     ##    ### ###    ${r}
    // ${b}##    ### ##    ##        /   ##    ##   ${w}##   ##     ##      ### /##  ${r}
    // ${b}########  ##    ##       /    ##    ##   ${w}##   ##     ##        #/ /## ${r}
    // ${b}#######   ##    ##      ###   ##    ##   ${w} ##  ##     ##         #/ ## ${r}
    // ${b}##        ##    ##       ###  ##    ##   ${w}  ## #      /           # /  ${r}
    // ${b}####    / ##    ##        ### ##    /#   ${w}   ###     /  /##        /   ${r}
    // ${b} ######/  ### / ### /      ##  ####/ ##  ${w}    ######/  /  ########/    ${r}
    // ${b}  #####    ##/   ##/       ##   ###   ## ${w}      ###   /     #####      ${r}
    // ${b}                           /             ${w}            |                ${r}
    // ${b}                          /              ${w}             \)              ${r}
    // ${b}                         /               ${w}                             ${r}
    // ${b}                        /                ${w}                             ${r}
    // `,

    //     // Banner 3
    //     `
    // ${b}      :::::::::::::      ::::::::::::::::::::    ::: ${w}    ::::::::  :::::::: ${r}
    // ${b}     :+:       :+:          :+:         :+:   :+: :+:${w}  :+:    :+::+:    :+: ${r}
    // ${b}    +:+       +:+          +:+        +:+   +:+   +:+${w} +:+    +:++:+         ${r}
    // ${b}   +#++:++#  +#+          +#+       +#+   +#++:++#++:${w}+#+    +:++#++:++#++   ${r}
    // ${b}  +#+       +#+          +#+      +#+    +#+     +#+${w}+#+    +#+       +#+    ${r}
    // ${b} #+#       #+#          #+#     #+#     #+#     #+##${w}+#    #+##+#    #+#     ${r}
    // ${b}##########################################     #### ${w}#######  ########       ${r}`,

    `;
    ${b}⠀⠀⠀⠀⠀⠀⠀⠀⢀⣐⣿⣿⢰⡀⠀⠀⠀${w} ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀${w}⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀${r}
    ${b}⠀⠀⠀⠀⠀⢀⣴⠤⠾⠛⠛⣿⣶⣇⠀⠀⡆${w} ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀${w}⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀${r}
    ${b}⢰⣋⡳⡄⠀⢨⣭⡀⠀⡤⠀⣀⣝⢿⣶⣿⡅${w} ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀${w}⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀${r}
    ${b}⢸⣯⠀⣇⠀⣼⣿⣿⣆⢷⣴⣿⣿⡏⣛⡉⠀${w} ⢸⣿⣿⣿⣿⣿⣿⢸⣿⣿⠀⠀⠀⠀⠀⣿⣿⡇⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⣾⣿⣿⣧⠀⠀⠀${w}⢸⠟⢀⣴⣿⣿⣿⣿⣦⡀⣠⣾⣿⣿⣿⣿⣦⡙⢿${r}
    ${b}⠀⠙⢷⣮⢸⣿⣿⣿⣿⣷⣯⣟⣏⣼⣷⣅⠾${w} ⢸⣿⣇⣀⣀⣀⠀⢸⣿⣿⠀⠀⠀⠀⠀⣿⣿⡇⠀⠀⠀⣠⣿⣿⠟⠁⠀⠀⣼⣿⡟⣿⣿⣆⠀⠀${w}⠀⠀⣿⣿⠋⠀⠈⠻⣿⡇⣿⣿⣅⣀⣀⡛⠛⠃⠀${r}
    ${b}⠀⠀⠀⠁⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠋⠀${w} ⢸⣿⡿⠿⠿⠿⠀⢸⣿⣿⠀⠀⠀⠀⠀⣿⣿⡇⠀⣠⣾⣿⠟⠁⠀⠀⠀⣰⣿⣿⣁⣸⣿⣿⡄⠀${w}⠀⠀⣿⣿ ⠀⠀ ⣿⣿⢈⣛⠿⠿⠿⣿⣷⡄⠀${r}
    ${b}⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣉⡟⠀⠀${w} ⢸⣿⣧⣤⣤⣤⣤⢸⣿⣿⣦⣤⣤⣤⡄⣿⣿⡇⣾⣿⣿⣧⣤⣤⣤⡄⢰⣿⣿⠟⠛⠛⠻⣿⣿⡄${w}⢠⡀⠻⣿⣿⣦⣴⣿⣿⠇⢿⣿⣦⣤⣤⣿⣿⠇⣠${r}
    ${b}⠀⠀⠀⠀⢰⡈⠛⠿⣿⣿⣿⣿⣿⠋⠀  ${w} ⠘⠛⠛⠛⠛⠛⠛⠈⠛⠛⠛⠛⠛⠛⠃⠛⠛⠃⠛⠛⠛⠛⠛⠛⠛⠃⠛⠛⠃⠀⠀⠀⠀⠙⠛⠃${w}⠘⠛⠀⠈⠛⠛⠛⠛⠁⠀⠀⠙⠛⠛⠛⠛⠁⠚⠛${r}
    ${b}⠀⠀⠀⠀⢸⣿⡦⠀⠀⠉⠛⠿⠃⠀⠀⠀ ${w} ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀${w}⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀${r}
    `,
    ];

    // Randomly select and log one banner
    const auto randomBanner = banners[Math.floor(Math.random() * banners.size())];

    std::cout << randomBanner << std::endl;

    if (version) {
        // log the version
        std::cout << versionColor + "Version: " + version + r << std::endl;
    }

    // Notify user if a new CLI version is available (unless we're skipping it)
    if (!skipUpdateCheck) {
        try {
            checkAndShowUpdateNotification(version);
            } catch (error) {
                // Silently continue if update check fails
            }
        }

}

} // namespace elizaos
