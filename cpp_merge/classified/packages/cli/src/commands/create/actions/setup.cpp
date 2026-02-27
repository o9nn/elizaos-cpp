#include "setup.hpp"

std::shared_ptr<Promise<void>> createProjectDirectories(string targetDir)
{
    std::async([=]() { ensureElizaDir(targetDir); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> setupAIModelConfig(string aiModel, string envFilePath, boolean isNonInteractive)
{
    try
    {
        static switch_type __switch823_5511 = {
            { any(std:("local")), 1 },
            { any(std:("openai")), 2 },
            { any(std:("claude")), 3 },
            { any(std:("openrouter")), 4 },
            { any(std:("google")), 5 }
        };
        switch (__switch823_5511[aiModel])
        {
        case 1:
            {
                if (isNonInteractive) {
                    auto content = string_empty;
                    if (existsSync(envFilePath)) {
                        content = std::async([=]() { fs->readFile(envFilePath, std:("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std:("\
"))))) {
                        content += std:("\
");
                    }
                    content += std:("\
# Local AI Configuration (using Ollama)\
");
                    content += std:("OLLAMA_API_ENDPOINT=http://localhost:11434\
");
                    content += std:("OLLAMA_MODEL=gemma3\
");
                    content += std:("OLLAMA_EMBEDDING_MODEL=nomic-embed-text\
");
                    content += std:("USE_OLLAMA_TEXT_MODELS=true\
");
                    content += std:("# Make sure Ollama is installed and running: https://ollama.ai/\
");
                    content += std:("# Pull models with: ollama pull gemma3 && ollama pull nomic-embed-text\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                } else {
                    std::async([=]() { promptAndStoreOllamaConfig(envFilePath); });
                    std::async([=]() { promptAndStoreOllamaEmbeddingConfig(envFilePath); });
                }
                break;
            }
        case 2:
            {
                if (isNonInteractive) {
                    auto content = string_empty;
                    if (existsSync(envFilePath)) {
                        content = std::async([=]() { fs->readFile(envFilePath, std:("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std:("\
"))))) {
                        content += std:("\
");
                    }
                    content += std:("\
# AI Model Configuration\
");
                    content += std:("# OpenAI Configuration\
");
                    content += std:("OPENAI_API_KEY=your_openai_api_key_here\
");
                    content += std:("# Get your API key from: https://platform.openai.com/api-keys\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                } else {
                    std::async([=]() { promptAndStoreOpenAIKey(envFilePath); });
                }
                break;
            }
        case 3:
            {
                if (isNonInteractive) {
                    auto content = string_empty;
                    if (existsSync(envFilePath)) {
                        content = std::async([=]() { fs->readFile(envFilePath, std:("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std:("\
"))))) {
                        content += std:("\
");
                    }
                    content += std:("\
# AI Model Configuration\
");
                    content += std:("# Anthropic API Configuration\
");
                    content += std:("ANTHROPIC_API_KEY=your_anthropic_api_key_here\
");
                    content += std:("# Get your API key from: https://console.anthropic.com/\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                } else {
                    std::async([=]() { promptAndStoreAnthropicKey(envFilePath); });
                }
                break;
            }
        case 4:
            {
                if (isNonInteractive) {
                    auto content = string_empty;
                    if (existsSync(envFilePath)) {
                        content = std::async([=]() { fs->readFile(envFilePath, std:("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std:("\
"))))) {
                        content += std:("\
");
                    }
                    content += std:("\
# AI Model Configuration\
");
                    content += std:("# OpenRouter Configuration\
");
                    content += std:("OPENROUTER_API_KEY=your_openrouter_api_key_here\
");
                    content += std:("# Get your API key from: https://openrouter.ai/keys\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                } else {
                    std::async([=]() { promptAndStoreOpenRouterKey(envFilePath); });
                }
                break;
            }
        case 5:
            {
                if (isNonInteractive) {
                    auto content = string_empty;
                    if (existsSync(envFilePath)) {
                        content = std::async([=]() { fs->readFile(envFilePath, std:("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std:("\
"))))) {
                        content += std:("\
");
                    }
                    content += std:("\
# AI Model Configuration\
");
                    content += std:("# Google Generative AI Configuration\
");
                    content += std:("GOOGLE_GENERATIVE_AI_API_KEY=your_google_api_key_here\
");
                    content += std:("# Get your API key from: https://aistudio.google.com/apikey\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                } else {
                    std::async([=]() { promptAndStoreGoogleKey(envFilePath); });
                }
                break;
            }
        default:
            console->warn(std:("Unknown AI model: ") + aiModel + std:(", skipping configuration"));
            return std::shared_ptr<Promise<void>>();
        }
    }
    catch (const any& error)
    {
        auto errorMessage = (is<Error>(error)) ? any(error->message) (std:("Unknown error"));
        console->error(std:("Failed to set up AI model configuration: ") + errorMessage + string_empty);
    }
};


boolean hasValidApiKey(string content, string keyName)
{
    auto regex = std::make_shared<RegExp>(std:("^") + keyName + std:("=(.+)$"), std:("m"));
    auto match = content->match(regex);
    if (!match) return false;
    auto value = (*const_(match))[1]->trim();
    return (AND((AND((AND((AND((value != string_empty), (!value->includes(std:("your_"))))), (!value->includes(std:("_here"))))), (!value->includes(std:("PLACEHOLDER"))))), (!value->includes(std:("placeholder")))));
};


std::shared_ptr<Promise<void>> setupEmbeddingModelConfig(string embeddingModel, string envFilePath, boolean isNonInteractive)
{
    try
    {
        auto content = string_empty;
        if (existsSync(envFilePath)) {
            content = std::async([=]() { fs->readFile(envFilePath, std:("utf8")); });
        }
        if (AND((content), (!content->endsWith(std:("\
"))))) {
            content += std:("\
");
        }
        static switch_type __switch6713_10427 = {
            { any(std:("local")), 1 },
            { any(std:("openai")), 2 },
            { any(std:("google")), 3 }
        };
        switch (__switch6713_10427[embeddingModel])
        {
        case 1:
            {
                if (!hasValidApiKey(content, std:("OLLAMA_API_ENDPOINT"))) {
                    if (isNonInteractive) {
                        if (!content->includes(std:("OLLAMA_API_ENDPOINT="))) {
                            content += std:("\
# Embedding Model Configuration (Fallback)\
");
                            content += std:("# Ollama Embeddings Configuration\
");
                            content += std:("OLLAMA_API_ENDPOINT=http://localhost:11434\
");
                            content += std:("OLLAMA_EMBEDDING_MODEL=nomic-embed-text\
");
                            content += std:("USE_OLLAMA_EMBEDDINGS=true\
");
                            content += std:("# Make sure Ollama is installed and running: https://ollama.ai/\
");
                        }
                        std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                    } else {
                        std::async([=]() { promptAndStoreOllamaEmbeddingConfig(envFilePath); });
                    }
                } else {
                    if (isNonInteractive) {
                        if (!content->includes(std:("OLLAMA_EMBEDDING_MODEL"))) {
                            content += std:("OLLAMA_EMBEDDING_MODEL=nomic-embed-text\
");
                        }
                        if (!content->includes(std:("USE_OLLAMA_EMBEDDINGS"))) {
                            content += std:("USE_OLLAMA_EMBEDDINGS=true\
");
                        }
                        std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                    } else {
                        std::async([=]() { promptAndStoreOllamaEmbeddingConfig(envFilePath); });
                    }
                }
                break;
            }
        case 2:
            {
                if (!hasValidApiKey(content, std:("OPENAI_API_KEY"))) {
                    if (isNonInteractive) {
                        if (!content->includes(std:("OPENAI_API_KEY="))) {
                            content += std:("\
# Embedding Model Configuration (Fallback)\
");
                            content += std:("# OpenAI Embeddings Configuration\
");
                            content += std:("OPENAI_API_KEY=your_openai_api_key_here\
");
                            content += std:("# Get your API key from: https://platform.openai.com/api-keys\
");
                        }
                        std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                    } else {
                        std::async([=]() { promptAndStoreOpenAIKey(envFilePath); });
                    }
                }
                break;
            }
        case 3:
            {
                if (!hasValidApiKey(content, std:("GOOGLE_GENERATIVE_AI_API_KEY"))) {
                    if (isNonInteractive) {
                        if (!content->includes(std:("GOOGLE_GENERATIVE_AI_API_KEY="))) {
                            content += std:("\
# Embedding Model Configuration (Fallback)\
");
                            content += std:("# Google Generative AI Embeddings Configuration\
");
                            content += std:("GOOGLE_GENERATIVE_AI_API_KEY=your_google_api_key_here\
");
                            content += std:("# Get your API key from: https://aistudio.google.com/apikey\
");
                        }
                        std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                    } else {
                        std::async([=]() { promptAndStoreGoogleKey(envFilePath); });
                    }
                }
                break;
            }
        default:
            console->warn(std:("Unknown embedding model: ") + embeddingModel + std:(", skipping configuration"));
            return std::shared_ptr<Promise<void>>();
        }
    }
    catch (const any& error)
    {
        auto errorMessage = (is<Error>(error)) ? any(error->message) (std:("Unknown error"));
        console->error(std:("Failed to set up embedding model configuration: ") + errorMessage + string_empty);
    }
};


any resolveModelToPlugin(string modelName)
{
    auto modelToPlugin = object{
        object::pair{std:("openai"), std:("openai")}, 
        object::pair{std:("claude"), std:("anthropic")}, 
        object::pair{std:("anthropic"), std:("anthropic")}, 
        object::pair{std:("openrouter"), std:("openrouter")}, 
        object::pair{std:("local"), std:("ollama")}, 
        object::pair{std:("google"), std:("google-genai")}
    };
    return OR((const_(modelToPlugin)[modelName]), (nullptr));
};


std::shared_ptr<Promise<void>> installModelPlugin(string modelName, string targetDir, string purpose)
{
    auto pluginName = resolveModelToPlugin(modelName);
    if (!pluginName) {
        return std::shared_ptr<Promise<void>>();
    }
    std::async([=]() { installPluginWithSpinner(pluginName, targetDir, purpose); });
};


std::shared_ptr<Promise<void>> setupProjectEnvironment(string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    std::async([=]() { createProjectDirectories(targetDir); });
    auto envFilePath = string_empty + targetDir + std:("/.env");
    if (database == std:("postgres")) {
        if (!isNonInteractive) {
            std::async([=]() { promptAndStorePostgresUrl(envFilePath); });
        }
    } else if (database == std:("pglite")) {
        std::async([=]() { setupPgLite(undefined, string_empty + targetDir + std:("/.env"), targetDir); });
    }
    if (!isNonInteractive) {
        std::async([=]() { setupAIModelConfig(aiModel, envFilePath, isNonInteractive); });
        if (embeddingModel) {
            std::async([=]() { setupEmbeddingModelConfig(embeddingModel, envFilePath, isNonInteractive); });
        }
    }
    std::async([=]() { installModelPlugin(aiModel, targetDir, (aiModel == std:("local")) ? std:("for local AI") : string_empty); });
    if (AND((embeddingModel), (embeddingModel != std:("local")))) {
        auto aiPluginName = resolveModelToPlugin(aiModel);
        auto embeddingPluginName = resolveModelToPlugin(embeddingModel);
        if (AND((embeddingPluginName), (embeddingPluginName != aiPluginName))) {
            std::async([=]() { installModelPlugin(embeddingModel, targetDir, std:("for embeddings")); });
        }
    } else if (embeddingModel == std:("local")) {
        if (aiModel != std:("local")) {
            std::async([=]() { installModelPlugin(embeddingModel, targetDir, std:("for embeddings")); });
        }
    }
    return std::shared_ptr<Promise<void>>();
};


