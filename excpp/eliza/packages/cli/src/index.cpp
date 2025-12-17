#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check for --no-emoji flag early (before command parsing)
    if (process.argv.includes('--no-emoji')) {
        configureEmojis({ forceDisable: true });
    }

    // Check for --no-auto-install flag early (before command parsing)
    if (process.argv.includes('--no-auto-install')) {
        process.env.ELIZA_NO_AUTO_INSTALL = "true";
    }

    // For ESM modules we need to use import.meta.url instead of __dirname
    const auto __filename = fileURLToPath(import.meta.url);
    const auto __dirname = dirname(__filename);

    // Find package.json relative to the current file
    const auto packageJsonPath = path.resolve(__dirname, "../package.json");

    // Add a simple check in case the path is incorrect
    auto version = "0.0.0"; // Fallback version;
    if (!existsSync(packageJsonPath)) {
        } else {
            const auto packageJson = /* JSON.parse */ readFileSync(packageJsonPath, "utf-8");
            version = packageJson.version;
        }

        // Check for built-in flags that exit early (before preAction hook runs)
        const auto args = process.argv.slice(2);
        const auto isUpdateCommand = (std::find(args.begin(), args.end(), "update") != args.end());
        const auto willShowBanner = args.size() == 0;

        // Show update notification for all commands except:
        // - when banner will show (it handles its own notification)
        // - when running update command
        if (!willShowBanner && !isUpdateCommand) {
            const auto currentVersion = getVersion();
            checkAndShowUpdateNotification(currentVersion);
        }

        const auto program = new Command();
        .name("elizaos");
        .version(version, "-v, --version", "output the version number");
        .option("--no-emoji", "Disable emoji output");
        .option("--no-auto-install", "Disable automatic Bun installation");

        // Add global options but hide them from global help
        // They will still be passed to all commands for backward compatibility
        // Note: Removed --remote-url global option as it conflicts with subcommand options

        program;
        .addCommand(create);
        .addCommand(monorepo);
        .addCommand(plugins);
        .addCommand(agent);
        .addCommand(tee);
        .addCommand(start);
        .addCommand(update);
        .addCommand(test);
        .addCommand(env);
        .addCommand(dev);
        .addCommand(publish);

        // if no args are passed, display the banner (it will handle its own update check)
        if (process.argv.length == 2) {
            displayBanner(false); // Let banner handle update check and show enhanced notification;
        }

        program.parseAsync();

}

} // namespace elizaos
