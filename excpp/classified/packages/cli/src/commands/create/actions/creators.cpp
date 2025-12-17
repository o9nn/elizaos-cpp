#include "creators.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handleInteractiveConfiguration(const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envFilePath = std::to_string(targetDir) + "/.env";

    // Handle PostgreSQL configuration
    if (database == 'postgres') {
        promptAndStorePostgresUrl(envFilePath);
    }

    // Handle AI model configuration
    if (aiModel != 'local' || embeddingModel) {
        if (aiModel != 'local') {
            setupAIModelConfig(aiModel, envFilePath, false);
        }
        if (embeddingModel) {
            setupEmbeddingModelConfig(embeddingModel, envFilePath, false);
        }
    }

}

std::future<void> createPlugin(const std::string& pluginName, const std::string& targetDir, std::string pluginType = "full", auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Process and validate the plugin name
        const auto nameResult = processPluginName(pluginName);
        if (!nameResult.isValid) {
            throw std::runtime_error(nameResult.error || 'Invalid plugin name');
        }

        const auto processedName = nameResult.processedName!;
        // Add prefix to ensure plugin directory name follows convention
        const auto pluginDirName = processedName.startsWith('plugin-');
        ? processedName;
        "plugin-" + std::to_string(processedName)

        // Show warning if the final name differs from what the user entered
        if (pluginDirName != pluginName) {
            console.warn(
            "\nWarning: changing "" + std::to_string(pluginName) + "" to "" + std::to_string(pluginDirName) + "" to conform to plugin naming conventions\n"
            );
        }

        const auto pluginTargetDir = join(targetDir, pluginDirName);

        // Validate target directory
        const auto dirResult = validateTargetDirectory(pluginTargetDir);
        if (!dirResult.isValid) {
            throw std::runtime_error(dirResult.error || 'Invalid target directory');
        }

        if (!isNonInteractive) {
            const auto displayDir = getDisplayDirectory(targetDir);
            const auto confirmCreate = clack.confirm({;
                "Create plugin "" + std::to_string(pluginDirName) + "" in " + std::to_string(displayDir) + "?"
                });

                if (clack.isCancel(confirmCreate) || !confirmCreate) {
                    clack.cancel('Plugin creation cancelled.');
                    process.exit(0);
                }
            }

            withCleanupOnInterrupt(pluginTargetDir, pluginDirName, async () => {
                // Map plugin type to template name
                const auto templateName = pluginType == 'quick' ? 'plugin-quick' : 'plugin';

                runTasks([;
                createTask('Copying plugin template', () =>;
                copyTemplateUtil(templateName as 'plugin' | 'plugin-quick', pluginTargetDir);
                ),
                createTask('Installing dependencies', () => installDependenciesWithSpinner(pluginTargetDir)),
                ]);

                std::cout << "\n" + std::to_string(colors.green('✓')) + " Plugin "" + std::to_string(pluginDirName) + "" created successfully!" << std::endl;
                std::cout << "\nNext steps:" << std::endl;
                std::cout << "  cd " + std::to_string(pluginDirName) << std::endl;
                std::cout << "  bun run build   # Build the plugin" << std::endl;
                std::cout << "\n  Common commands:" << std::endl;
                std::cout << "  elizaos dev    # Start development mode with hot reloading" << std::endl;
                std::cout << "  elizaos start  # Start in production mode" << std::endl;
                std::cout << "\n" + std::to_string(colors.yellow('⚠️')) + "  Security reminder:" << std::endl;
                std::cout << "  - Check .gitignore is present before committing" << std::endl;
                std::cout << "  - Never commit .env files or API keys" << std::endl;
                std::cout << "  - Add sensitive files to .gitignore if needed\n" << std::endl;
                });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> createAgent(const std::string& agentName, const std::string& targetDir, auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto agentFilePath = std::to_string(agentName) + ".json";

        // Check if agent file already exists
        try {
            fs.access(agentFilePath);
            throw std::runtime_error(`Agent file ${agentFilePath} already exists`);
            } catch (error: any) {
                if (error.code != 'ENOENT') {
                    throw;
                }
                // File doesn't exist, which is what we want
            }

            if (!isNonInteractive) {
                const auto displayDir = getDisplayDirectory(targetDir);
                const auto confirmCreate = clack.confirm({;
                    "Create agent "" + std::to_string(agentName) + "" in " + std::to_string(displayDir) + "?"
                    });

                    if (clack.isCancel(confirmCreate) || !confirmCreate) {
                        clack.cancel('Agent creation cancelled.');
                        process.exit(0);
                    }
                }

                // Create agent character based on Eliza template
                const auto agentCharacter = {;
                    ...getElizaCharacter(),
                    name: agentName,
                    bio: [
                    std::to_string(agentName) + " is a helpful AI assistant created to provide assistance and engage in meaningful conversations."
                    std::to_string(agentName) + " is knowledgeable, creative, and always eager to help users with their questions and tasks."
                    ],
                    };

                    fs.writeFile(agentFilePath, JSON.stringify(agentCharacter, nullptr, 2));

                    // Always show success message and usage instructions - this is critical information
                    // that users need regardless of interactive/non-interactive mode
                    std::cout << "\n" + std::to_string(colors.green('✓')) + " Agent "" + std::to_string(agentName) + "" created successfully!" << std::endl;
                    std::cout << "Agent character created successfully at: " + std::to_string(agentFilePath) << std::endl;
                    std::cout << "\nTo use this agent:" << std::endl;
                    std::cout << "  1. Start ElizaOS server with this character:" << std::endl;
                    std::cout << "     elizaos start --character " + std::to_string(agentFilePath) << std::endl;
                    std::cout << "\n  OR if a server is already running:" << std::endl;
                    std::cout << "     elizaos agent start --path " + std::to_string(agentFilePath) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> createTEEProject(const std::string& projectName, const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Clear any inherited PGLITE_DATA_DIR to prevent child projects from inheriting parent's database
        delete process.env.PGLITE_DATA_DIR;

        const auto teeTargetDir = join(targetDir, projectName);

        // Validate target directory
        const auto dirResult = validateTargetDirectory(teeTargetDir);
        if (!dirResult.isValid) {
            throw std::runtime_error(dirResult.error || 'Invalid target directory');
        }

        if (!isNonInteractive) {
            const auto displayDir = getDisplayDirectory(targetDir);
            const auto confirmCreate = clack.confirm({;
                "Create TEE project "" + std::to_string(projectName) + "" in " + std::to_string(displayDir) + "?"
                });

                if (clack.isCancel(confirmCreate) || !confirmCreate) {
                    clack.cancel('TEE project creation cancelled.');
                    process.exit(0);
                }
            }

            withCleanupOnInterrupt(teeTargetDir, projectName, async () => {
                // Create project directory first
                fs.mkdir(teeTargetDir, { recursive: true });

                // Handle interactive configuration before spinner tasks
                if (!isNonInteractive) {
                    handleInteractiveConfiguration(teeTargetDir, database, aiModel, embeddingModel);
                }

                runTasks([;
                createTask('Copying TEE template', () =>;
                copyTemplateUtil('project-tee-starter', teeTargetDir);
                ),
                createTask('Setting up project environment', () =>;
                setupProjectEnvironment(teeTargetDir, database, aiModel, embeddingModel, true);
                ),
                createTask('Installing dependencies', () => installDependenciesWithSpinner(teeTargetDir)),
                createTask('Building project', () => buildProjectWithSpinner(teeTargetDir, false)),
                ]);

                std::cout << "\n" + std::to_string(colors.green('✓')) + " TEE project "" + std::to_string(projectName) + "" created successfully!" << std::endl;
                std::cout << "\nNext steps:" << std::endl;
                std::cout << "  cd " + std::to_string(projectName) << std::endl;
                std::cout << "\n  Common commands:" << std::endl;
                std::cout << "  elizaos dev    # Start development mode with hot reloading" << std::endl;
                std::cout << "  elizaos start  # Start in production mode\n" << std::endl;
                });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> createProject(const std::string& projectName, const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Clear any inherited PGLITE_DATA_DIR to prevent child projects from inheriting parent's database
        delete process.env.PGLITE_DATA_DIR;

        // Handle current directory case
        const auto projectTargetDir = projectName == '.' ? targetDir : join(targetDir, projectName);

        // Validate target directory
        const auto dirResult = validateTargetDirectory(projectTargetDir);
        if (!dirResult.isValid) {
            throw std::runtime_error(dirResult.error || 'Invalid target directory');
        }

        if (!isNonInteractive) {
            const auto displayDir = getDisplayDirectory(targetDir);
            const auto displayProjectName = "project "" + std::to_string(projectName) + """;
            const auto confirmCreate = clack.confirm({;
                "Create " + std::to_string(displayProjectName) + " in " + std::to_string(displayDir) + "?"
                });

                if (clack.isCancel(confirmCreate) || !confirmCreate) {
                    clack.cancel('Project creation cancelled.');
                    process.exit(0);
                }
            }

            // only use cleanup wrapper for new directories, not current directory
            const auto createFn = async () => {;
                // Create project directory first if it's not current directory
                if (projectName != '.') {
                    fs.mkdir(projectTargetDir, { recursive: true });
                }

                // Handle interactive configuration before spinner tasks
                if (!isNonInteractive) {
                    handleInteractiveConfiguration(projectTargetDir, database, aiModel, embeddingModel);
                }

                runTasks([;
                createTask('Copying project template', () =>;
                copyTemplateUtil('project-starter', projectTargetDir);
                ),
                createTask('Setting up project environment', () =>;
                setupProjectEnvironment(projectTargetDir, database, aiModel, embeddingModel, true);
                ),
                createTask('Installing dependencies', () => installDependenciesWithSpinner(projectTargetDir)),
                createTask('Building project', () => buildProjectWithSpinner(projectTargetDir, false)),
                ]);

                const auto displayName = "Project "" + std::to_string(projectName) + """;
                std::cout << "\n" + std::to_string(colors.green('✓')) + " " + std::to_string(displayName) + " initialized successfully!" << std::endl;
                std::cout << "\nNext steps:" << std::endl;
                if (projectName != '.') {
                    std::cout << "  cd " + std::to_string(projectName) << std::endl;
                }
                std::cout << "\n  Common commands:" << std::endl;
                std::cout << "  elizaos dev    # Start development mode with hot reloading" << std::endl;
                std::cout << "  elizaos start  # Start in production mode\n" << std::endl;
                };

                if (projectName == '.') {
                    // for current directory, no cleanup needed
                    createFn();
                    } else {
                        // for new directory, use cleanup wrapper
                        withCleanupOnInterrupt(projectTargetDir, projectName, createFn);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
