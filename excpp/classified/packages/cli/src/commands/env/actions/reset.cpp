#include "reset.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> resolvePgliteDir() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Default PGLite directory path
    return path.join(process.cwd(), ".eliza", "db");

}

std::future<void> resetEnv(ResetEnvOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { yes } = options;

    // Get all relevant paths
    const auto elizaDir = path.join(process.cwd(), ".eliza");
    const auto cacheDir = path.join(elizaDir, "cache");

    const auto localEnvPath = (getLocalEnvPath()) || path.join(process.cwd(), ".env");
    const auto localDbDir = resolvePgliteDir();

    // Create reset item options
    const std::vector<ResetItem> resetItems = [;
    {
        title: "Local environment variables",
        value: "localEnv",
        description: existsSync(localEnvPath)
        ? "Reset values in local .env file";
        : "Local .env file not found, nothing to reset",
        selected: existsSync(localEnvPath),
        },
        {
            title: "Cache folder",
            value: "cache",
            description: existsSync(cacheDir)
            ? "Delete the cache folder";
            : "Cache folder not found, nothing to delete",
            selected: existsSync(cacheDir),
            },
            {
                title: "Local database files",
                value: "localDb",
                description: existsSync(localDbDir)
                ? "Delete local database files";
                : "Local database folder not found, nothing to delete",
                selected: existsSync(localDbDir),
                },
                ];

                // Filter out non-existent items for automated selection
                const auto validResetItems = resetItems.filter(;
                (item) =>;
                (item.value == "localEnv" && existsSync(localEnvPath)) ||;
                (item.value == "cache" && existsSync(cacheDir)) ||;
                (item.value == "localDb" && existsSync(localDbDir));
                );

                // Get selected items (from options or defaults)
                std::vector<ResetTarget> selectedValues = [];

                if (yes) {
                    // When using --yes flag, include all valid reset items
                    selectedValues = validResetItems.map((item) => item.value);

                    // Show what will be reset
                    if (selectedValues.length > 0) {
                        std::cout << colors.bold("The following items will be reset:") << std::endl;
                        for (const auto& value : selectedValues)
                            const auto item = resetItems.find((item) => item.value == value);
                            std::cout << "  • " + std::to_string(item.title || value) << std::endl;
                        }
                        } else {
                            std::cout << "No valid items found to reset." << std::endl;
                            return;
                        }
                        } else {
                            // Prompt user to select items with styling matching interactive mode
                            const auto selections = clack.multiselect({;
                                message: colors.cyan(colors.bold("Select items to reset:")),
                                options: resetItems.map((item) => ({ value: item.value, label: item.title })),
                                required: true,
                                });

                                if (clack.isCancel(selections)) {
                                    clack.cancel("Operation cancelled.");
                                    process.exit(0);
                                }

                                if (!selections || selections.length == 0) {
                                    std::cout << "No items selected. Reset cancelled." << std::endl;
                                    return;
                                }

                                selectedValues = selections;

                                // Show selected items
                                std::cout << "\nYou selected:" << std::endl;
                                for (const auto& value : selectedValues)
                                    const auto item = resetItems.find((item) => item.value == value);
                                    std::cout << "  • " + std::to_string(item.title || value) << std::endl;
                                }

                                // Final confirmation
                                const auto confirm = clack.confirm({;
                                    message: "Are you sure you want to reset the selected items?",
                                    initialValue: false,
                                    });

                                    if (clack.isCancel(confirm)) {
                                        clack.cancel("Operation cancelled.");
                                        process.exit(0);
                                    }

                                    if (!confirm) {
                                        std::cout << "Reset cancelled." << std::endl;
                                        return;
                                    }
                                }

                                // Track reset results
                                const ResetActionRecord actions = {;
                                    reset: [],
                                    deleted: [],
                                    skipped: [],
                                    warning: [],
                                    };

                                    // Process each selected item
                                    for (const auto& target : selectedValues)
                                        switch (target) {
                                            // case "localEnv":
                                            if (await resetEnvFile(localEnvPath)) {
                                                actions.reset.push_back("Local environment variables");
                                                } else {
                                                    actions.skipped.push_back("Local environment variables (no file or empty)");
                                                }
                                                break;

                                                // case "cache":
                                                safeDeleteDirectory(cacheDir, actions, "Cache folder");
                                                break;

                                                // case "localDb":
                                                safeDeleteDirectory(localDbDir, actions, "Local database folder");
                                                break;
                                            }
                                        }

                                        // Print summary report
                                        std::cout << colors.bold("\nReset Summary:") << std::endl;

                                        if (actions.reset.length > 0) {
                                            std::cout << colors.green("  Values Cleared:") << std::endl;
                                            actions.reset.forEach((item) => console.log(`    • ${item}`));
                                        }

                                        if (actions.deleted.length > 0) {
                                            std::cout << colors.green("  Deleted:") << std::endl;
                                            actions.deleted.forEach((item) => console.log(`    • ${item}`));
                                        }

                                        if (actions.skipped.length > 0) {
                                            std::cout << colors.yellow("  Skipped:") << std::endl;
                                            actions.skipped.forEach((item) => console.log(`    • ${item}`));
                                        }

                                        if (actions.warning.length > 0) {
                                            std::cout << colors.red("  Warnings:") << std::endl;
                                            actions.warning.forEach((item) => console.log(`    • ${item}`));
                                        }

                                        std::cout << colors.bold("\nEnvironment reset complete") << std::endl;

}

} // namespace elizaos
