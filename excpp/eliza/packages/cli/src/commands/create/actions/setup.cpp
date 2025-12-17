#include "setup.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> createProjectDirectories(const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    ensureElizaDir(targetDir);

}

std::future<void> setupAIModelConfig(const std::string& aiModel, const std::string& envFilePath, auto isNonInteractive = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        switch (aiModel) {
            case 'local': {
                std::cout << '[√] Using Local AI - no additional configuration needed' << std::endl;
                break;
            }

            case 'openai': {
                if (isNonInteractive) {
                    // In non-interactive mode, just add placeholder
                    auto content = '';
                    if (existsSync(envFilePath)) {
                        content = fs.readFile(envFilePath, 'utf8');
                    }

                    if (content && !content.endsWith('\n')) {
                        content += '\n';
                    }

                    content += '\n# AI Model Configuration\n';
                    content += '# OpenAI Configuration\n';
                    content += 'OPENAI_API_KEY=your_openai_api_key_here\n';
                    content += '# Get your API key from: https://platform.openai.com/api-keys\n';

                    fs.writeFile(envFilePath, content, 'utf8');
                    std::cout << '[√] OpenAI placeholder configuration added to .env file' << std::endl;
                    } else {
                        // Interactive mode - prompt for OpenAI API key
                        promptAndStoreOpenAIKey(envFilePath);
                    }
                    break;
                }

                case 'claude': {
                    if (isNonInteractive) {
                        // In non-interactive mode, just add placeholder
                        auto content = '';
                        if (existsSync(envFilePath)) {
                            content = fs.readFile(envFilePath, 'utf8');
                        }

                        if (content && !content.endsWith('\n')) {
                            content += '\n';
                        }

                        content += '\n# AI Model Configuration\n';
                        content += '# Anthropic API Configuration\n';
                        content += 'ANTHROPIC_API_KEY=your_anthropic_api_key_here\n';
                        content += '# Get your API key from: https://console.anthropic.com/\n';

                        fs.writeFile(envFilePath, content, 'utf8');
                        std::cout << '[√] Anthropic API placeholder configuration added to .env file' << std::endl;
                        } else {
                            // Interactive mode - prompt for Anthropic API key
                            promptAndStoreAnthropicKey(envFilePath);
                        }
                        break;
                    }

                    case 'openrouter': {
                        if (isNonInteractive) {
                            // In non-interactive mode, just add placeholder
                            auto content = '';
                            if (existsSync(envFilePath)) {
                                content = fs.readFile(envFilePath, 'utf8');
                            }

                            if (content && !content.endsWith('\n')) {
                                content += '\n';
                            }

                            content += '\n# AI Model Configuration\n';
                            content += '# OpenRouter Configuration\n';
                            content += 'OPENROUTER_API_KEY=your_openrouter_api_key_here\n';
                            content += '# Get your API key from: https://openrouter.ai/keys\n';

                            fs.writeFile(envFilePath, content, 'utf8');
                            std::cout << '[√] OpenRouter placeholder configuration added to .env file' << std::endl;
                            } else {
                                // Interactive mode - prompt for OpenRouter API key
                                promptAndStoreOpenRouterKey(envFilePath);
                            }
                            break;
                        }

                        case 'ollama': {
                            if (isNonInteractive) {
                                // In non-interactive mode, just add placeholder
                                auto content = '';
                                if (existsSync(envFilePath)) {
                                    content = fs.readFile(envFilePath, 'utf8');
                                }

                                if (content && !content.endsWith('\n')) {
                                    content += '\n';
                                }

                                content += '\n# AI Model Configuration\n';
                                content += '# Ollama Configuration\n';
                                content += 'OLLAMA_API_ENDPOINT=http://localhost:11434\n';
                                content += 'OLLAMA_MODEL=llama2\n';
                                content += 'USE_OLLAMA_TEXT_MODELS=true\n';
                                content += '# Make sure Ollama is installed and running: https://ollama.ai/\n';

                                fs.writeFile(envFilePath, content, 'utf8');
                                std::cout << '[√] Ollama placeholder configuration added to .env file' << std::endl;
                                } else {
                                    // Interactive mode - prompt for Ollama configuration
                                    promptAndStoreOllamaConfig(envFilePath);
                                }
                                break;
                            }

                            case 'google': {
                                if (isNonInteractive) {
                                    // In non-interactive mode, just add placeholder
                                    auto content = '';
                                    if (existsSync(envFilePath)) {
                                        content = fs.readFile(envFilePath, 'utf8');
                                    }

                                    if (content && !content.endsWith('\n')) {
                                        content += '\n';
                                    }

                                    content += '\n# AI Model Configuration\n';
                                    content += '# Google Generative AI Configuration\n';
                                    content += 'GOOGLE_GENERATIVE_AI_API_KEY=your_google_api_key_here\n';
                                    content += '# Get your API key from: https://aistudio.google.com/apikey\n';

                                    fs.writeFile(envFilePath, content, 'utf8');
                                    std::cout << '[√] Google Generative AI placeholder configuration added to .env file' << std::endl;
                                    } else {
                                        // Interactive mode - prompt for Google API key
                                        promptAndStoreGoogleKey(envFilePath);
                                    }
                                    break;
                                }

                                default:
                                std::cout << "Unknown AI model: " + std::to_string(aiModel) << skipping configuration` << std::endl;
                                return;
                            }
                            } catch (error: unknown) {
                                const auto errorMessage = true /* instanceof check */ ? error.message : 'Unknown error';
                                std::cerr << "Failed to set up AI model configuration: " + std::to_string(errorMessage) << std::endl;
                            }

}

bool hasValidApiKey(const std::string& content, const std::string& keyName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto regex = "^" + std::to_string(keyName) + "=(.+)$";
    const auto match = content.match(regex);
    if (!match) return false;

    const auto value = match[1].trim();
    // Check if it's not empty and not a placeholder
    return (;
    value != '' &&;
    !value.includes('your_') &&;
    !value.includes('_here') &&;
    !value.includes('PLACEHOLDER') &&;
    !value.includes('placeholder');
    );

}

std::future<void> setupEmbeddingModelConfig(const std::string& embeddingModel, const std::string& envFilePath, auto isNonInteractive = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        auto content = '';
        if (existsSync(envFilePath)) {
            content = fs.readFile(envFilePath, 'utf8');
        }

        if (content && !content.endsWith('\n')) {
            content += '\n';
        }

        switch (embeddingModel) {
            case 'local': {
                content += '\n# Embedding Model Configuration (Fallback)\n';
                content += '# Using local embeddings - no additional configuration needed\n';
                fs.writeFile(envFilePath, content, 'utf8');
                std::cout << '[√] Using Local embeddings - no additional configuration needed' << std::endl;
                break;
            }

            case 'openai': {
                // Check if OpenAI key already exists with a valid value
                if (!hasValidApiKey(content, 'OPENAI_API_KEY')) {
                    if (isNonInteractive) {
                        // In non-interactive mode, add/update placeholder
                        if (!content.includes('OPENAI_API_KEY=')) {
                            content += '\n# Embedding Model Configuration (Fallback)\n';
                            content += '# OpenAI Embeddings Configuration\n';
                            content += 'OPENAI_API_KEY=your_openai_api_key_here\n';
                            content += '# Get your API key from: https://platform.openai.com/api-keys\n';
                        }
                        fs.writeFile(envFilePath, content, 'utf8');
                        std::cout << '[√] OpenAI embeddings placeholder configuration added to .env file' << std::endl;
                        } else {
                            // Interactive mode - prompt for OpenAI API key
                            std::cout << '\n[!] OpenAI API key is required for embeddings' << std::endl;
                            promptAndStoreOpenAIKey(envFilePath);
                        }
                        } else {
                            std::cout << '[√] OpenAI API key already configured - will use for embeddings' << std::endl;
                        }
                        break;
                    }

                    case 'ollama': {
                        // Check if Ollama config already exists with valid values
                        if (!hasValidApiKey(content, 'OLLAMA_API_ENDPOINT')) {
                            if (isNonInteractive) {
                                // In non-interactive mode, add/update placeholder
                                if (!content.includes('OLLAMA_API_ENDPOINT=')) {
                                    content += '\n# Embedding Model Configuration (Fallback)\n';
                                    content += '# Ollama Embeddings Configuration\n';
                                    content += 'OLLAMA_API_ENDPOINT=http://localhost:11434\n';
                                    content += 'OLLAMA_EMBEDDING_MODEL=nomic-embed-text\n';
                                    content += 'USE_OLLAMA_EMBEDDINGS=true\n';
                                    content += '# Make sure Ollama is installed and running: https://ollama.ai/\n';
                                }
                                fs.writeFile(envFilePath, content, 'utf8');
                                std::cout << '[√] Ollama embeddings placeholder configuration added to .env file' << std::endl;
                                } else {
                                    // Interactive mode - prompt for Ollama embedding model configuration
                                    std::cout << '\n[!] Ollama embedding model configuration is required' << std::endl;
                                    promptAndStoreOllamaEmbeddingConfig(envFilePath);
                                }
                                } else {
                                    // Ollama endpoint exists, but we need to prompt for embedding model specifically
                                    if (isNonInteractive) {
                                        // In non-interactive mode, just add embedding model if not present
                                        if (!content.includes('OLLAMA_EMBEDDING_MODEL')) {
                                            content += 'OLLAMA_EMBEDDING_MODEL=nomic-embed-text\n';
                                        }
                                        if (!content.includes('USE_OLLAMA_EMBEDDINGS')) {
                                            content += 'USE_OLLAMA_EMBEDDINGS=true\n';
                                        }
                                        fs.writeFile(envFilePath, content, 'utf8');
                                        std::cout << '[√] Ollama embedding model configuration added to .env file' << std::endl;
                                        } else {
                                            // Interactive mode - always prompt for embedding model selection
                                            std::cout << '\n[!] Please select an Ollama embedding model' << std::endl;
                                            promptAndStoreOllamaEmbeddingConfig(envFilePath);
                                        }
                                    }
                                    break;
                                }

                                case 'google': {
                                    // Check if Google API key already exists with a valid value
                                    if (!hasValidApiKey(content, 'GOOGLE_GENERATIVE_AI_API_KEY')) {
                                        if (isNonInteractive) {
                                            // In non-interactive mode, add/update placeholder
                                            if (!content.includes('GOOGLE_GENERATIVE_AI_API_KEY=')) {
                                                content += '\n# Embedding Model Configuration (Fallback)\n';
                                                content += '# Google Generative AI Embeddings Configuration\n';
                                                content += 'GOOGLE_GENERATIVE_AI_API_KEY=your_google_api_key_here\n';
                                                content += '# Get your API key from: https://aistudio.google.com/apikey\n';
                                            }
                                            fs.writeFile(envFilePath, content, 'utf8');
                                            std::cout << '[√] Google embeddings placeholder configuration added to .env file' << std::endl;
                                            } else {
                                                // Interactive mode - prompt for Google API key
                                                std::cout << '\n[!] Google Generative AI API key is required for embeddings' << std::endl;
                                                promptAndStoreGoogleKey(envFilePath);
                                            }
                                            } else {
                                                std::cout << '[√] Google API key already configured - will use for embeddings' << std::endl;
                                            }
                                            break;
                                        }

                                        default:
                                        std::cout << "Unknown embedding model: " + std::to_string(embeddingModel) << skipping configuration` << std::endl;
                                        return;
                                    }
                                    } catch (error: unknown) {
                                        const auto errorMessage = true /* instanceof check */ ? error.message : 'Unknown error';
                                        std::cerr << "Failed to set up embedding model configuration: " + std::to_string(errorMessage) << std::endl;
                                    }

}

std::future<void> installDependencies(const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Skip dependency installation in CI/test environments to save memory and time
    if (process.env.CI == 'true' || process.env.ELIZA_TEST_MODE == 'true') {
        std::cout << 'Skipping dependency installation in CI/test environment...' << std::endl;
        return;
    }

    std::cout << 'Installing dependencies...' << std::endl;
    runBunCommand(['install'], targetDir);

}

std::future<void> setupProjectEnvironment(const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create project directories first
    createProjectDirectories(targetDir);

    // Set up database configuration
    const auto envFilePath = std::to_string(targetDir) + "/.env";
    if (database == 'postgres' && !isNonInteractive) {
        promptAndStorePostgresUrl(envFilePath);
        } else if (database == 'pglite') {
            std::to_string(targetDir) + "/.env";
        }

        // Set up AI model configuration
        setupAIModelConfig(aiModel, envFilePath, isNonInteractive);

        // Set up embedding model configuration if needed
        if (embeddingModel) {
            setupEmbeddingModelConfig(embeddingModel, envFilePath, isNonInteractive);
        }

}

} // namespace elizaos
