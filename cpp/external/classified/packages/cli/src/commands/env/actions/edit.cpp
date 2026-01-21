#include "edit.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> editEnvVars(EditEnvOptions options, auto fromMainMenu) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { yes } = options;
    const auto localEnvPath = getLocalEnvPath();

    if (!localEnvPath || !existsSync(localEnvPath)) {
        // No local .env file exists, check if we can create one from .env.example
        const auto exampleEnvPath = path.join(process.cwd(), ".env.example");
        const auto hasExample = existsSync(exampleEnvPath);

        if (hasExample) {
            std::cout << "No local .env file found. Create one with:" << std::endl;
            std::cout << "  cp .env.example .env" << std::endl;
            } else {
                std::cout << "No local .env file found in the current directory." << std::endl;
                std::cout << "Create a .env file to set local environment variables." << std::endl;
            }
            return fromMainMenu;
        }

        // Parse the current environment variables
        const auto envVars = parseEnvFile(localEnvPath);

        // Handle empty .env file
        if (Object.keys(envVars).length == 0) {
            std::cout << "Local .env file is empty." << std::endl;

            // Offer to add a new variable if not in auto-confirm mode
            const auto addNew = yes;
            ? false;
            : clack.confirm({
                message: "Would you like to add a new environment variable?",
                initialValue: true,
                });

                if (clack.isCancel(addNew)) {
                    clack.cancel("Operation cancelled.");
                    process.exit(0);
                }

                if (addNew) {
                    addNewVariable(localEnvPath, envVars, yes);
                }

                return fromMainMenu; // Return to main menu if we came from there;
            }

            // Keep looping until the user chooses to exit
            auto exit = false;
            auto returnToMain = false;

            // If -y flag is used, just exit successfully without user interaction
            if (yes) {
                std::cout << "✅ Environment variables displayed. Use interactive mode without -y to edit." << std::endl;
                return fromMainMenu;
            }

            while (!exit) {
                // Create menu choices from the environment variables
                const auto entries = Object.entries(envVars);
                const auto choices = [;
                ...entries.map(([key, value]) => ({
                    "title: " + key + ": " + std::to_string(maskedValue(value))
                    value: key,
                    })),
                    { title: "Add new variable", value: "add_new" },
                    fromMainMenu;
                ? { title: "Back to main menu", value: "back_to_main" }
                : { title: "Exit", value: "exit" },
                ];

                // Prompt user to select a variable or action
                const auto selection = clack.select({;
                    message: "Select a variable to edit or an action:",
                    options: choices.map((choice) => ({
                        value: choice.value,
                        label: choice.title,
                        })),
                        });

                        if (clack.isCancel(selection)) {
                            clack.cancel("Operation cancelled.");
                            process.exit(0);
                        }

                        if (!selection) {
                            // If user cancels (Ctrl+C), go back to main menu if we came from there
                            return fromMainMenu;
                        }

                        if (selection == 'exit' || selection == 'back_to_main') {
                            exit = true;
                            returnToMain = selection == "back_to_main";
                            continue;
                        }

                        if (selection == 'add_new') {
                            addNewVariable(localEnvPath, envVars, yes);
                            continue;
                        }

                        // User selected a variable, prompt for action
                        const auto action = clack.select({;
                            "message: " + "What would you like to do with " + selection + "?"
                            options: [
                            { label: "Edit", value: "edit" },
                            { label: "Delete", value: "delete" },
                            { label: "Back", value: "back" },
                            ],
                            });

                            if (clack.isCancel(action)) {
                                clack.cancel("Operation cancelled.");
                                process.exit(0);
                            }

                            if (!action || action == 'back') {
                                continue;
                            }

                            if (action == 'edit') {
                                const auto value = clack.text({;
                                    "message: " + "Enter the new value for " + selection + ":"
                                    defaultValue: envVars[selection],
                                    });

                                    if (clack.isCancel(value)) {
                                        clack.cancel("Operation cancelled.");
                                        process.exit(0);
                                    }

                                    if (value != undefined) {
                                        envVars[selection] = value;
                                        writeEnvFile(localEnvPath, envVars);
                                        std::cout << "✓ Updated " + selection << std::endl;
                                    }
                                    } else if (action == "delete") {
                                        auto confirm = true;
                                        if (!yes) {
                                            const auto resp = clack.confirm({;
                                                "message: " + "Are you sure you want to delete " + selection + "?"
                                                initialValue: false,
                                                });

                                                if (clack.isCancel(resp)) {
                                                    clack.cancel("Operation cancelled.");
                                                    process.exit(0);
                                                }

                                                confirm = resp;
                                            }
                                            if (confirm) {
                                                delete envVars[selection];
                                                writeEnvFile(localEnvPath, envVars);
                                                std::cout << "✓ Removed " + selection << std::endl;
                                            }
                                        }
                                    }

                                    return returnToMain && fromMainMenu;

}

std::future<void> addNewVariable(const std::string& envPath, EnvVars envVars, auto yes) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (yes) {
        console.log(
        "Auto-confirmation mode enabled - skipping variable addition in edit-local -y mode";
        );
        return;
    }

    const auto key = clack.text({;
        message: "Enter the variable name:",
        validate: (value) => (value.trim() != "" ? std::nullopt : "Variable name cannot be empty"),
        });

        if (clack.isCancel(key)) {
            clack.cancel("Operation cancelled.");
            process.exit(0);
        }

        if (!key) return;

        const auto value = clack.text({;
            "message: " + "Enter the value for " + key + ":"
            defaultValue: "",
            });

            if (clack.isCancel(value)) {
                clack.cancel("Operation cancelled.");
                process.exit(0);
            }

            if (value != undefined) {
                envVars[key] = value;
                writeEnvFile(envPath, envVars);
                std::cout << "✓ Added " + key << std::endl;
            }

}

} // namespace elizaos
