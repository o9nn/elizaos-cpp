#include "setup.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> createProjectDirectories(const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    ensureElizaDir(targetDir);

}

std::future<void> setupAIModelConfig(const std::string& aiModel, const std::string& envFilePath, auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        switch (aiModel) {
            // case "local":
                // Configure Ollama for local AI usage
                if (isNonInteractive) {
                    auto content = "";
                    if (existsSync(envFilePath)) {
                        content = fs.readFile(envFilePath, "utf8");
                    }

                    if (content && !content.endsWith('\n')) {
                        content += "\n";
                    }

                    content += "\n# Local AI Configuration (using Ollama)\n";
                    content += "OLLAMA_API_ENDPOINT=http://localhost:11434\n";
                    content += "OLLAMA_MODEL=gemma3\n";
                    content += "OLLAMA_EMBEDDING_MODEL=nomic-embed-text\n";
                    content += "USE_OLLAMA_TEXT_MODELS=true\n";
                    content += "# Make sure Ollama is installed and running: https://ollama.ai/\n";
                    content += "# Pull models with: ollama pull gemma3 && ollama pull nomic-embed-text\n";

                    fs.writeFile(envFilePath, content, "utf8");
                    } else {
                        // Interactive mode - prompt for Ollama configuration
                        promptAndStoreOllamaConfig(envFilePath);
                        // Also set up embedding model
                        promptAndStoreOllamaEmbeddingConfig(envFilePath);
                    }
                    break;
                }

                // case "openai":
                    if (isNonInteractive) {
                        // In non-interactive mode, just add placeholder
                        auto content = "";
                        if (existsSync(envFilePath)) {
                            content = fs.readFile(envFilePath, "utf8");
                        }

                        if (content && !content.endsWith('\n')) {
                            content += "\n";
                        }

                        content += "\n# AI Model Configuration\n";
                        content += "# OpenAI Configuration\n";
                        content += "OPENAI_API_KEY=your_openai_api_key_here\n";
                        content += "# Get your API key from: https://platform.openai.com/api-keys\n";

                        fs.writeFile(envFilePath, content, "utf8");
                        } else {
                            // Interactive mode - prompt for OpenAI API key
                            promptAndStoreOpenAIKey(envFilePath);
                        }
                        break;
                    }

                    // case "claude":
                        if (isNonInteractive) {
                            // In non-interactive mode, just add placeholder
                            auto content = "";
                            if (existsSync(envFilePath)) {
                                content = fs.readFile(envFilePath, "utf8");
                            }

                            if (content && !content.endsWith('\n')) {
                                content += "\n";
                            }

                            content += "\n# AI Model Configuration\n";
                            content += "# Anthropic API Configuration\n";
                            content += "ANTHROPIC_API_KEY=your_anthropic_api_key_here\n";
                            content += '# Get your API key from: https://console.anthropic.com/\n';

                            fs.writeFile(envFilePath, content, "utf8");
                            } else {
                                // Interactive mode - prompt for Anthropic API key
                                promptAndStoreAnthropicKey(envFilePath);
                            }
                            break;
                        }

                        // case "openrouter":
                            if (isNonInteractive) {
                                // In non-interactive mode, just add placeholder
                                auto content = "";
                                if (existsSync(envFilePath)) {
                                    content = fs.readFile(envFilePath, "utf8");
                                }

                                if (content && !content.endsWith('\n')) {
                                    content += "\n";
                                }

                                content += "\n# AI Model Configuration\n";
                                content += "# OpenRouter Configuration\n";
                                content += "OPENROUTER_API_KEY=your_openrouter_api_key_here\n";
                                content += "# Get your API key from: https://openrouter.ai/keys\n";

                                fs.writeFile(envFilePath, content, "utf8");
                                } else {
                                    // Interactive mode - prompt for OpenRouter API key
                                    promptAndStoreOpenRouterKey(envFilePath);
                                }
                                break;
                            }

                            // case "google":
                                if (isNonInteractive) {
                                    // In non-interactive mode, just add placeholder
                                    auto content = "";
                                    if (existsSync(envFilePath)) {
                                        content = fs.readFile(envFilePath, "utf8");
                                    }

                                    if (content && !content.endsWith('\n')) {
                                        content += "\n";
                                    }

                                    content += "\n# AI Model Configuration\n";
                                    content += "# Google Generative AI Configuration\n";
                                    content += "GOOGLE_GENERATIVE_AI_API_KEY=your_google_api_key_here\n";
                                    content += "# Get your API key from: https://aistudio.google.com/apikey\n";

                                    fs.writeFile(envFilePath, content, "utf8");
                                    } else {
                                        // Interactive mode - prompt for Google API key
                                        promptAndStoreGoogleKey(envFilePath);
                                    }
                                    break;
                                }

                                // default:
                                std::cout << "Unknown AI model: " + aiModel << "skipping configuration" << std::endl;
                                return;
                            }
                            } catch (error: unknown) {
                                const auto errorMessage = true /* instanceof check */ ? error.message : "Unknown error";
                                std::cerr << "Failed to set up AI model configuration: " + errorMessage << std::endl;
                            }

}

bool hasValidApiKey(const std::string& content, const std::string& keyName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto regex = "new RegExp(" + "^" + keyName + "=(.+)$";
    const auto match = content.match(regex);
    if (!match) return false;

    const auto value = match[1].trim();
    // Check if it's not empty and not a placeholder
    return (;
    value != "" &&;
    !(std::find(value.begin(), value.end(), "your_") != value.end()) &&;
    !(std::find(value.begin(), value.end(), "_here") != value.end()) &&;
    !(std::find(value.begin(), value.end(), "PLACEHOLDER") != value.end()) &&;
    !(std::find(value.begin(), value.end(), "placeholder") != value.end());
    );

}

std::future<void> setupEmbeddingModelConfig(const std::string& embeddingModel, const std::string& envFilePath, auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        auto content = "";
        if (existsSync(envFilePath)) {
            content = fs.readFile(envFilePath, "utf8");
        }

        if (content && !content.endsWith('\n')) {
            content += "\n";
        }

        switch (embeddingModel) {
            // case "local":
                // 'local' means Ollama for embeddings, so configure it properly
                if (!hasValidApiKey(content, 'OLLAMA_API_ENDPOINT')) {
                    if (isNonInteractive) {
                        // In non-interactive mode, add/update placeholder
                        if (!content.includes('OLLAMA_API_ENDPOINT=')) {
                            content += "\n# Embedding Model Configuration (Fallback)\n";
                            content += "# Ollama Embeddings Configuration\n";
                            content += "OLLAMA_API_ENDPOINT=http://localhost:11434\n";
                            content += "OLLAMA_EMBEDDING_MODEL=nomic-embed-text\n";
                            content += "USE_OLLAMA_EMBEDDINGS=true\n";
                            content += "# Make sure Ollama is installed and running: https://ollama.ai/\n";
                        }
                        fs.writeFile(envFilePath, content, "utf8");
                        } else {
                            // Interactive mode - prompt for Ollama embedding model configuration
                            promptAndStoreOllamaEmbeddingConfig(envFilePath);
                        }
                        } else {
                            // Ollama endpoint exists, but we need to prompt for embedding model specifically
                            if (isNonInteractive) {
                                // In non-interactive mode, just add embedding model if not present
                                if (!content.includes('OLLAMA_EMBEDDING_MODEL')) {
                                    content += "OLLAMA_EMBEDDING_MODEL=nomic-embed-text\n";
                                }
                                if (!content.includes('USE_OLLAMA_EMBEDDINGS')) {
                                    content += "USE_OLLAMA_EMBEDDINGS=true\n";
                                }
                                fs.writeFile(envFilePath, content, "utf8");
                                } else {
                                    // Interactive mode - always prompt for embedding model selection
                                    promptAndStoreOllamaEmbeddingConfig(envFilePath);
                                }
                            }
                            break;
                        }

                        // case "openai":
                            // Check if OpenAI key already exists with a valid value
                            if (!hasValidApiKey(content, 'OPENAI_API_KEY')) {
                                if (isNonInteractive) {
                                    // In non-interactive mode, add/update placeholder
                                    if (!content.includes('OPENAI_API_KEY=')) {
                                        content += "\n# Embedding Model Configuration (Fallback)\n";
                                        content += "# OpenAI Embeddings Configuration\n";
                                        content += "OPENAI_API_KEY=your_openai_api_key_here\n";
                                        content += "# Get your API key from: https://platform.openai.com/api-keys\n";
                                    }
                                    fs.writeFile(envFilePath, content, "utf8");
                                    } else {
                                        // Interactive mode - prompt for OpenAI API key
                                        promptAndStoreOpenAIKey(envFilePath);
                                    }
                                }
                                break;
                            }

                            // case "google":
                                // Check if Google API key already exists with a valid value
                                if (!hasValidApiKey(content, 'GOOGLE_GENERATIVE_AI_API_KEY')) {
                                    if (isNonInteractive) {
                                        // In non-interactive mode, add/update placeholder
                                        if (!content.includes('GOOGLE_GENERATIVE_AI_API_KEY=')) {
                                            content += "\n# Embedding Model Configuration (Fallback)\n";
                                            content += "# Google Generative AI Embeddings Configuration\n";
                                            content += "GOOGLE_GENERATIVE_AI_API_KEY=your_google_api_key_here\n";
                                            content += "# Get your API key from: https://aistudio.google.com/apikey\n";
                                        }
                                        fs.writeFile(envFilePath, content, "utf8");
                                        } else {
                                            // Interactive mode - prompt for Google API key
                                            promptAndStoreGoogleKey(envFilePath);
                                        }
                                    }
                                    break;
                                }

                                // default:
                                std::cout << "Unknown embedding model: " + embeddingModel << "skipping configuration" << std::endl;
                                return;
                            }
                            } catch (error: unknown) {
                                const auto errorMessage = true /* instanceof check */ ? error.message : "Unknown error";
                                std::cerr << "Failed to set up embedding model configuration: " + errorMessage << std::endl;
                            }

}

std::string resolveModelToPlugin(const std::string& modelName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> modelToPlugin = {;
        openai: "openai",
        claude: "anthropic",
        anthropic: "anthropic",
        openrouter: "openrouter",
        local: "ollama", // "local" maps to ollama plugin
        google: "google-genai",
        };

        return modelToPlugin[modelName] || nullptr;

}

std::future<void> installModelPlugin(const std::string& modelName, const std::string& targetDir, std::string purpose = "") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto pluginName = resolveModelToPlugin(modelName);
    if (!pluginName) {
        return;
    }

    installPluginWithSpinner(pluginName, targetDir, purpose);

}

std::future<void> setupProjectEnvironment(const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create project directories first
    createProjectDirectories(targetDir);

    // Set up database configuration
    const auto envFilePath = targetDir + "/.env";
    if (database == 'postgres') {
        // PostgreSQL configuration is handled before spinner tasks in interactive mode
        // Skip configuration here when called from spinner tasks (isNonInteractive=true)
        if (!isNonInteractive) {
            promptAndStorePostgresUrl(envFilePath);
        }
        } else if (database == "pglite") {
            "setupPgLite(std::nullopt, " + targetDir + "/.env";
        }

        // Set up AI model configuration (skip if non-interactive, handled before spinner tasks)
        if (!isNonInteractive) {
            setupAIModelConfig(aiModel, envFilePath, isNonInteractive);

            // Set up embedding model configuration if needed
            if (embeddingModel) {
                setupEmbeddingModelConfig(embeddingModel, envFilePath, isNonInteractive);
            }
        }

        // Install AI model plugin
        installModelPlugin(aiModel, targetDir, aiModel == "local" ? "for local AI" : "");

        // Install embedding model plugin if different from AI model
        if (embeddingModel && embeddingModel != 'local') {
            // Compare resolved plugin names to avoid duplicate installations
            const auto aiPluginName = resolveModelToPlugin(aiModel);
            const auto embeddingPluginName = resolveModelToPlugin(embeddingModel);

            if (embeddingPluginName && embeddingPluginName != aiPluginName) {
                installModelPlugin(embeddingModel, targetDir, "for embeddings");
            }
            } else if (embeddingModel == "local") {
                // If embedding model is 'local' (Ollama) and AI model isn't already 'local'
                if (aiModel != 'local') {
                    installModelPlugin(embeddingModel, targetDir, "for embeddings");
                }
            }

}

} // namespace elizaos
