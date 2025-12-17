#include "generate.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> generatePlugin(GeneratePluginOptions opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Lazy import to avoid loading dependencies until needed
            const auto { PluginCreator } = import('@/src/utils/plugin-creator.js');

            // Set API key if provided
            if (opts.apiKey) {
                process.env.ANTHROPIC_API_KEY = opts.apiKey;
            }

            // Check for API key
            if (!process.env.ANTHROPIC_API_KEY) {
                std::cerr << 'ANTHROPIC_API_KEY is required for plugin generation.' << std::endl;
                std::cout << '\nPlease set ANTHROPIC_API_KEY environment variable or use --api-key option.' << std::endl;
                process.exit(1);
            }

            // Handle spec file if provided
            auto spec = std::nullopt;
            if (opts.specFile) {
                try {
                    const auto specContent = readFileSync(opts.specFile, 'utf-8');
                    spec = JSON.parse(specContent);
                    } catch (error) {
                        logger.error(
                        "Failed to read or parse spec file: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                        );
                        process.exit(1);
                    }
                    } else if (opts.skipPrompts) {
                        std::cerr << '--skip-prompts requires --spec-file to be provided' << std::endl;
                        process.exit(1);
                    }

                    // Create creator instance with options
                    const auto creator = new PluginCreator({;
                        skipTests: opts.skipTests,
                        skipValidation: opts.skipValidation,
                        skipPrompts: opts.skipPrompts,
                        spec: spec,
                        });

                        // Run generation
                        std::cout << "\n" + std::to_string(emoji.rocket('Starting AI-powered plugin generation...')) + "\n" << std::endl;
                        const GenerationResult result = creator.create();

                        if (result.success) {
                            std::cout << "\n" + std::to_string(emoji.success('Plugin successfully generated!')) << std::endl;
                            std::cout << "   Name: " + std::to_string(result.pluginName) << std::endl;
                            std::cout << "   Location: " + std::to_string(result.pluginPath) << std::endl;
                            std::cout << "\nThe plugin has been created in your current directory." << std::endl;
                            std::cout << "\nNext steps:" << std::endl;
                            std::cout << "1. cd " + std::to_string(path.basename(result.pluginPath || '')) << std::endl;
                            std::cout << "2. Review the generated code" << std::endl;
                            std::cout << "3. Test the plugin: bun test" << std::endl;
                            std::cout << "4. Add to your ElizaOS project" << std::endl;
                            } else {
                                std::cerr << "Plugin generation failed: " + std::to_string(result.error.message) << std::endl;
                                process.exit(1);
                            }
                            } catch (error) {
                                handleError(error);
                                process.exit(1);
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
