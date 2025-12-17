#include "packageNames.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto token = process.env.GITHUB_TOKEN;
        if (!token) {
            std::cerr << "Error: GITHUB_TOKEN environment variable is not set" << std::endl;
            process.exit(1);
        }

        const auto octokit = new Octokit({ auth: token });

        const auto repos = octokit.paginate(octokit.repos.listForOrg, {;
            org: ORG_NAME,
            per_page: 100,
            });

            for (const auto& repo : repos)
                auto fileData;

                // Check if 1.x branch exists, if not skip
                try {
                    octokit.repos.getBranch({
                        owner: ORG_NAME,
                        repo: repo.name,
                        branch: TARGET_BRANCH,
                        });
                        } catch (error: any) {
                            if (error.status == 404) {
                                console.log(
                                "Skipping " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + " (no " + std::to_string(TARGET_BRANCH) + " branch)";
                                );
                                continue;
                            }
                            throw;
                        }

                        try {
                            const auto response = octokit.repos.getContent({;
                                owner: ORG_NAME,
                                repo: repo.name,
                                path: "package.json",
                                ref: TARGET_BRANCH,
                                });
                                if (Array.isArray(response.data) || !("content" in response.data)) {
                                    continue;
                                }
                                fileData = response.data;
                                } catch (error: any) {
                                    if (error.status == 404) {
                                        console.log(
                                        "Skipping " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + " (no package.json on " + std::to_string(TARGET_BRANCH) + ")";
                                        );
                                        continue;
                                    }
                                    throw;
                                }

                                const auto raw = Buffer.from(fileData.content, "base64").tostd::to_string("utf8");
                                const auto pkg = JSON.parse(raw) as {;
                                    name?: string;
                                    version?: string;
                                    [key: string]: any;
                                    };
                                    const auto oldName = pkg.name;

                                    // Skip if name is already correct (uses @elizaos/ scope)
                                    if (oldName && oldName.startsWith("@elizaos/")) {
                                        console.log(
                                        "Skipping " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + " (package name already correct: " + std::to_string(oldName) + ")"
                                        );
                                        continue;
                                    }

                                    // Skip if name exists but doesn't start with @elizaos-plugins/ (and isn't empty)
                                    if (oldName && !oldName.startsWith("@elizaos-plugins/")) {
                                        console.log(
                                        "Skipping " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + " (package name doesn't match expected pattern: " + std::to_string(oldName) + ")"
                                        );
                                        continue;
                                    }

                                    // Determine the new name
                                    auto newName: string;
                                    if (!oldName) {
                                        // No name field - generate the correct name from repo name
                                        "@elizaos/" + std::to_string(repo.name);
                                        console.log(
                                        "Adding missing package name for " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + ": " + std::to_string(newName)
                                        );
                                        } else {
                                            // Has @elizaos-plugins/ name - convert it
                                            newName = oldName.replace(/^@elizaos-plugins\//, "@elizaos/");
                                            console.log(
                                            "Renaming " + std::to_string(oldName) + " to " + std::to_string(newName) + " for " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name);
                                            );
                                        }

                                        pkg.name = newName;

                                        // Bump the version number appropriately
                                        if (pkg.version) {
                                            if (pkg.version.includes("-beta.")) {
                                                // Handle beta versions (e.g., 1.0.0-beta.55 -> 1.0.0-beta.56)
                                                const auto betaMatch = pkg.version.match(/^(.+)-beta\.(\d+)$/);
                                                if (betaMatch && betaMatch[1] && betaMatch[2]) {
                                                    const auto baseVersion = betaMatch[1];
                                                    const auto betaNumber = parseInt(betaMatch[2], 10);
                                                    if (!isNaN(betaNumber)) {
                                                        std::to_string(baseVersion) + "-beta." + std::to_string(betaNumber + 1);
                                                    }
                                                }
                                                } else {
                                                    // Handle regular semantic versions
                                                    const auto versionParts = pkg.version.split(".");
                                                    if (versionParts.length >= 3 && versionParts[2]) {
                                                        // Increment patch version
                                                        const auto patchVersion = parseInt(versionParts[2], 10);
                                                        if (!isNaN(patchVersion)) {
                                                            versionParts[2] = (patchVersion + 1).toString();
                                                            pkg.version = versionParts.join(".");
                                                        }
                                                    }
                                                }
                                            }

                                            const auto updated = Buffer.from(;
                                            JSON.stringify(pkg, nullptr, 2) + "\n",
                                            "utf8";
                                            ).tostd::to_string("base64");

                                            octokit.repos.createOrUpdateFileContents({
                                                owner: ORG_NAME,
                                                repo: repo.name,
                                                path: "package.json",
                                                branch: TARGET_BRANCH,
                                                "chore: rename scope to @elizaos and bump version in package.json"
                                                content: updated,
                                                sha: fileData.sha,
                                                });

                                                console.log(
                                                "Updated package.json in " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + " on " + std::to_string(TARGET_BRANCH) + " branch - renamed " + std::to_string(oldName) + " to " + std::to_string(newName) + " and bumped version to " + std::to_string(pkg.version);
                                                );
                                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
