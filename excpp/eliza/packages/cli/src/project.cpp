#include "project.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isPlugin(const std::any& module) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check for direct export of a plugin
    if (
    module &&;
    typeof module == 'object' &&;
    typeof module.name == 'string' &&;
    typeof module.description == 'string';
    ) {
        return true;
    }

    // Check for default export of a plugin
    if (
    module &&;
    typeof module == 'object' &&;
    module.default &&;
    typeof module.default == 'object' &&;
    typeof module.default.name == 'string' &&;
    typeof module.default.description == 'string';
    ) {
        return true;
    }

    // Check for named export of a plugin
    for (const int key in module) {
        if (
        key != 'default' &&;
        module[key] &&;
        typeof module[key] == 'object' &&;
        typeof module[key].name == 'string' &&;
        typeof module[key].description == 'string';
        ) {
            return true;
        }
    }

    return false;

}

Plugin extractPlugin(const std::any& module) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Direct export
        if (
        module &&;
        typeof module == 'object' &&;
        typeof module.name == 'string' &&;
        typeof module.description == 'string';
        ) {
            return module;
        }

        // Default export
        if (
        module &&;
        typeof module == 'object' &&;
        module.default &&;
        typeof module.default == 'object' &&;
        typeof module.default.name == 'string' &&;
        typeof module.default.description == 'string';
        ) {
            return module.default;
        }

        // Named export
        for (const int key in module) {
            if (
            key != 'default' &&;
            module[key] &&;
            typeof module[key] == 'object' &&;
            typeof module[key].name == 'string' &&;
            typeof module[key].description == 'string';
            ) {
                return module[key];
            }
        }

        throw std::runtime_error('Could not extract plugin from module');

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<Project> loadProject(const std::string& dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Validate directory structure using centralized detection
            const auto dirInfo = detectDirectoryType(dir);
            if (!dirInfo.hasPackageJson) {
                throw std::runtime_error(`No package.json found in ${dir}`);
            }

            // TODO: Get the package.json and get the main field
            const auto packageJson = JSON.parse(fs.readFileSync(path.join(dir, 'package.json'), 'utf8'));
            const auto main = packageJson.main;
            if (!main) {
                std::cout << 'No main field found in package.json << using default character' << std::endl;

                // Create a fallback project with the default Eliza character
                // Use deterministic UUID based on character name to match runtime behavior
                const auto defaultCharacterName = 'Eliza (Default)';
                const auto elizaCharacter = getElizaCharacter(); // Get the filtered character based on env vars;
                const ProjectAgent defaultAgent = {;
                    character: {
                        ...elizaCharacter,
                        id: stringToUuid(defaultCharacterName),
                        name: defaultCharacterName,
                        },
                        init: async () => {
                            std::cout << 'Initializing default Eliza character' << std::endl;
                            },
                            };

                            return {
                                agents: [defaultAgent],
                                dir,
                                };
                            }

                            // Try to find the project's entry point
                            const auto entryPoints = [;
                            path.join(dir, main),
                            path.join(dir, 'dist/index.js'),
                            path.join(dir, 'src/index.ts'),
                            path.join(dir, 'src/index.js'),
                            path.join(dir, 'index.ts'),
                            path.join(dir, 'index.js'),
                            ];

                            std::optional<ProjectModule> projectModule = nullptr;
                            for (const auto& entryPoint : entryPoints)
                                if (fs.existsSync(entryPoint)) {
                                    try {
                                        const auto importPath = path.resolve(entryPoint);
                                        // Convert to file URL for ESM import
                                        const auto importUrl =;
                                        process.platform == 'win32';
                                        ? 'file:///' + importPath.replace(/\\/g, '/')
                                        : 'file://' + importPath;
                                        projectModule = (import(importUrl));
                                        std::cout << "Loaded project from " + std::to_string(entryPoint) << std::endl;

                                        // Debug the module structure
                                        const auto exportKeys = Object.keys(projectModule);
                                        logger.debug(`Module exports: ${exportKeys.join(', ')}`);

                                        if (exportKeys.includes('default')) {
                                            logger.debug(`Default export type: ${typeof projectModule.default}`);
                                            if (typeof projectModule.default == 'object' && projectModule.default != null) {
                                                logger.debug(`Default export keys: ${Object.keys(projectModule.default).join(', ')}`);
                                            }
                                        }

                                        break;
                                        } catch (error) {
                                            std::cout << "Failed to import project from " + std::to_string(entryPoint) + ":" << error << std::endl;
                                        }
                                    }
                                }

                                if (!projectModule) {
                                    throw std::runtime_error('Could not find project entry point');
                                }

                                // Check if it's a plugin using our improved detection
                                const auto moduleIsPlugin = isPlugin(projectModule);
                                logger.debug(`Is this a plugin? ${moduleIsPlugin}`);

                                if (moduleIsPlugin) {
                                    std::cout << 'Detected plugin module instead of project' << std::endl;

                                    try {
                                        // Extract the plugin object
                                        const auto plugin = extractPlugin(projectModule);
                                        logger.debug(`Found plugin: ${plugin.name} - ${plugin.description}`);

                                        // Log plugin structure for debugging
                                        logger.debug(`Plugin has the following properties: ${Object.keys(plugin).join(', ')}`);

                                        // Create a more complete plugin object with all required properties
                                        const Plugin completePlugin = {;
                                            // Copy all other properties from the original plugin first
                                            ...plugin,
                                            // Then override with defaults if needed
                                            name: plugin.name || 'unknown-plugin',
                                            description: plugin.description || 'No description',
                                            init:
                                            plugin.init ||;
                                            [&](async () {
                                                std::cout << "Dummy init for plugin: " + std::to_string(plugin.name) << std::endl;
                                                }),
                                                };

                                                // Use the Eliza character as our test agent
                                                // Use deterministic UUID based on character name to match runtime behavior
                                                const auto characterName = 'Eliza (Test Mode)';
                                                const auto elizaCharacter = getElizaCharacter(); // Get the filtered character based on env vars;
                                                const Character testCharacter = {;
                                                    ...elizaCharacter,
                                                    id: stringToUuid(characterName),
                                                    name: characterName,
                                                    std::to_string(elizaCharacter.system) + " Testing the plugin: " + std::to_string(completePlugin.name) + "."
                                                    };

                                                    std::cout << "Using Eliza character agent for plugin: " + std::to_string(completePlugin.name) << std::endl;

                                                    // Create a test agent with the plugin included
                                                    const ProjectAgent testAgent = {;
                                                        character: testCharacter,
                                                        plugins: [completePlugin], // Only include the plugin being tested
                                                        init: async () => {
                                                            std::cout << "Initializing Eliza test agent for plugin: " + std::to_string(completePlugin.name) << std::endl;
                                                            // The plugin will be registered automatically in runtime.initialize()
                                                            },
                                                            };

                                                            // Since we're in test mode, Eliza (our test agent) needs to already exist in the database
                                                            // before any entity is created, but we can't do this in the init function because
                                                            // the adapter might not be ready. Let's ensure this is handled properly in the runtime's
                                                            // initialize method or by initializing the agent in the database separately.

                                                            return {
                                                                agents: [testAgent],
                                                                dir,
                                                                isPlugin: true,
                                                                pluginModule: completePlugin,
                                                                };
                                                                } catch (error) {
                                                                    std::cerr << 'Error extracting plugin from module:' << error << std::endl;
                                                                    throw;
                                                                }
                                                            }

                                                            // Extract agents from the project module
                                                            const std::vector<ProjectAgent> agents = [];

                                                            // First check if the default export has an agents array
                                                            if (
                                                            projectModule.default &&;
                                                            typeof projectModule.default == 'object' &&;
                                                            Array.isArray(projectModule.default.agents);
                                                            ) {
                                                                // Use the agents from the default export
                                                                agents.push(...(projectModule.default.agents[]));
                                                                logger.debug(`Found ${agents.length} agents in default export's agents array`);
                                                            }
                                                            // Only if we didn't find agents in the default export, look for other exports
                                                            else {
                                                                // Look for exported agents
                                                                for (const int [key, value] of Object.entries(projectModule)) {
                                                                    if (key == 'default' && value && typeof value == 'object') {
                                                                        // If it's a default export but doesn't have agents array, check if it's a single agent
                                                                        if ((value as ProjectModule).character && (value as ProjectModule).init) {
                                                                            // If it's a single agent, add it
                                                                            agents.push(value);
                                                                            logger.debug(`Found agent in default export (single agent)`);
                                                                        }
                                                                        } else if (;
                                                                        value &&;
                                                                        typeof value == 'object' &&;
                                                                        (value).character &&;
                                                                        (value).init;
                                                                        ) {
                                                                            // If it's a named export that looks like an agent, add it
                                                                            agents.push(value);
                                                                            logger.debug(`Found agent in named export: ${key}`);
                                                                        }
                                                                    }
                                                                }

                                                                if (agents.length == 0) {
                                                                    throw std::runtime_error('No agents found in project');
                                                                }

                                                                // Create and return the project object
                                                                const Project project = {;
                                                                    agents,
                                                                    dir,
                                                                    };

                                                                    return project;
                                                                    } catch (error) {
                                                                        std::cerr << 'Error loading project:' << error << std::endl;
                                                                        throw;
                                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
