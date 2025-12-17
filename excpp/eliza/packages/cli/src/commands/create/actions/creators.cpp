#include "creators.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> createPlugin(const std::string& pluginName, const std::string& targetDir, auto isNonInteractive) {
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
        const auto pluginTargetDir = join(targetDir, pluginDirName);

        // Validate target directory
        const auto dirResult = validateTargetDirectory(pluginTargetDir);
        if (!dirResult.isValid) {
            throw std::runtime_error(dirResult.error || 'Invalid target directory');
        }

        if (!isNonInteractive) {
            const auto confirmCreate = clack.confirm({;
                "Create plugin "" + std::to_string(pluginDirName) + "" in " + std::to_string(pluginTargetDir) + "?"
                });

                if (clack.isCancel(confirmCreate) || !confirmCreate) {
                    clack.cancel('Plugin creation cancelled.');
                    process.exit(0);
                }
            }

            // Copy plugin template
            copyTemplateUtil('plugin', pluginTargetDir);

            // Install dependencies
            installDependencies(pluginTargetDir);

            std::cout << "\n" + std::to_string(colors.green('✓')) + " Plugin "" + std::to_string(pluginDirName) + "" created successfully!" << std::endl;
            std::cout << "\nNext steps:" << std::endl;
            std::cout << "  cd " + std::to_string(pluginDirName) << std::endl;
            std::cout << "  bun run build" << std::endl;
            std::cout << "  bun run test\n" << std::endl;

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
                const auto confirmCreate = clack.confirm({;
                    "Create agent "" + std::to_string(agentName) + "" at " + std::to_string(agentFilePath) + "?"
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

                    if (!isNonInteractive) {
                        std::cout << "\n" + std::to_string(colors.green('✓')) + " Agent "" + std::to_string(agentName) + "" created successfully!" << std::endl;
                    }
                    std::cout << "Agent character created successfully at: " + std::to_string(agentFilePath) << std::endl;
                    std::cout << "\nTo use this agent:" << std::endl;
                    std::cout << "  elizaos agent start --path " + std::to_string(agentFilePath) + "\n" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> createTEEProject(const std::string& projectName, const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto teeTargetDir = join(targetDir, projectName);

        // Validate target directory
        const auto dirResult = validateTargetDirectory(teeTargetDir);
        if (!dirResult.isValid) {
            throw std::runtime_error(dirResult.error || 'Invalid target directory');
        }

        if (!isNonInteractive) {
            const auto confirmCreate = clack.confirm({;
                "Create TEE project "" + std::to_string(projectName) + "" in " + std::to_string(teeTargetDir) + "?"
                });

                if (clack.isCancel(confirmCreate) || !confirmCreate) {
                    clack.cancel('TEE project creation cancelled.');
                    process.exit(0);
                }
            }

            // Copy TEE template
            copyTemplateUtil('project-tee-starter', teeTargetDir);

            // Set up project environment
            setupProjectEnvironment(teeTargetDir, database, aiModel, embeddingModel, isNonInteractive);

            // Install dependencies
            installDependencies(teeTargetDir);

            // Build the project
            buildProject(teeTargetDir);

            std::cout << "\n" + std::to_string(colors.green('✓')) + " TEE project "" + std::to_string(projectName) + "" created successfully!" << std::endl;
            std::cout << "\nNext steps:" << std::endl;
            std::cout << "  cd " + std::to_string(projectName) << std::endl;
            std::cout << "  bun run dev\n" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> createProject(const std::string& projectName, const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Handle current directory case
        const auto projectTargetDir = projectName == '.' ? targetDir : join(targetDir, projectName);

        // Validate target directory
        const auto dirResult = validateTargetDirectory(projectTargetDir);
        if (!dirResult.isValid) {
            throw std::runtime_error(dirResult.error || 'Invalid target directory');
        }

        if (!isNonInteractive) {
            const auto confirmCreate = clack.confirm({;
                "Create project "" + std::to_string(projectName) + "" in " + std::to_string(projectTargetDir) + "?"
                });

                if (clack.isCancel(confirmCreate) || !confirmCreate) {
                    clack.cancel('Project creation cancelled.');
                    process.exit(0);
                }
            }

            // Copy project template
            copyTemplateUtil('project-starter', projectTargetDir);

            // Set up project environment
            setupProjectEnvironment(;
            projectTargetDir,
            database,
            aiModel,
            embeddingModel,
            isNonInteractive;
            );

            // Install dependencies
            installDependencies(projectTargetDir);

            // Build the project
            buildProject(projectTargetDir);

            const auto displayName = "Project "" + std::to_string(projectName) + """;
            std::cout << "\n" + std::to_string(colors.green('✓')) + " " + std::to_string(displayName) + " initialized successfully!" << std::endl;
            std::cout << "\nNext steps:" << std::endl;
            std::cout << "  cd " + std::to_string(projectName) << std::endl;
            std::cout << "  bun run dev\n" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
