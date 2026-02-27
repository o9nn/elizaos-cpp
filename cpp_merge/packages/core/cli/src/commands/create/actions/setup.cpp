#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/create/actions/setup.h"

std::shared_ptr<Promise<void>> createProjectDirectories(string targetDir)
{
    std::async([=]() { ensureElizaDir(targetDir); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> setupAIModelConfig(string aiModel, string envFilePath, boolean isNonInteractive)
{
    try
    {
        static switch_type __switch770_5802 = {
            { any(std:("local")), 1 },
            { any(std:("openai")), 2 },
            { any(std:("claude")), 3 },
            { any(std:("openrouter")), 4 },
            { any(std:("ollama")), 5 },
            { any(std:("google")), 6 }
        };
        switch (__switch770_5802[aiModel])
        {
        case 1:
            {
                console->info(std:("[√] Using Local AI - no additional configuration needed"));
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
                    console->info(std:("[√] OpenAI placeholder configuration added to .env file"));
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
                    console->info(std:("[√] Anthropic API placeholder configuration added to .env file"));
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
                    console->info(std:("[√] OpenRouter placeholder configuration added to .env file"));
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
                    content += std:("# Ollama Configuration\
");
                    content += std:("OLLAMA_API_ENDPOINT=http://localhost:11434\
");
                    content += std:("OLLAMA_MODEL=llama2\
");
                    content += std:("USE_OLLAMA_TEXT_MODELS=true\
");
                    content += std:("# Make sure Ollama is installed and running: https://ollama.ai/\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                    console->info(std:("[√] Ollama placeholder configuration added to .env file"));
                } else {
                    std::async([=]() { promptAndStoreOllamaConfig(envFilePath); });
                }
                break;
            }
        case 6:
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
                    console->info(std:("[√] Google Generative AI placeholder configuration added to .env file"));
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
        static switch_type __switch6997_11974 = {
            { any(std:("local")), 1 },
            { any(std:("openai")), 2 },
            { any(std:("ollama")), 3 },
            { any(std:("google")), 4 }
        };
        switch (__switch6997_11974[embeddingModel])
        {
        case 1:
            {
                content += std:("\
# Embedding Model Configuration (Fallback)\
");
                content += std:("# Using local embeddings - no additional configuration needed\
");
                std::async([=]() { fs->writeFile(envFilePath, content, std:("utf8")); });
                console->info(std:("[√] Using Local embeddings - no additional configuration needed"));
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
                        console->info(std:("[√] OpenAI embeddings placeholder configuration added to .env file"));
                    } else {
                        console->info(std:("\
[!] OpenAI API key is required for embeddings"));
                        std::async([=]() { promptAndStoreOpenAIKey(envFilePath); });
                    }
                } else {
                    console->info(std:("[√] OpenAI API key already configured - will use for embeddings"));
                }
                break;
            }
        case 3:
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
                        console->info(std:("[√] Ollama embeddings placeholder configuration added to .env file"));
                    } else {
                        console->info(std:("\
[!] Ollama embedding model configuration is required"));
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
                        console->info(std:("[√] Ollama embedding model configuration added to .env file"));
                    } else {
                        console->info(std:("\
[!] Please select an Ollama embedding model"));
                        std::async([=]() { promptAndStoreOllamaEmbeddingConfig(envFilePath); });
                    }
                }
                break;
            }
        case 4:
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
                        console->info(std:("[√] Google embeddings placeholder configuration added to .env file"));
                    } else {
                        console->info(std:("\
[!] Google Generative AI API key is required for embeddings"));
                        std::async([=]() { promptAndStoreGoogleKey(envFilePath); });
                    }
                } else {
                    console->info(std:("[√] Google API key already configured - will use for embeddings"));
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


std::shared_ptr<Promise<void>> installDependencies(string targetDir)
{
    if (OR((process->env->CI == std:("true")), (process->env->ELIZA_TEST_MODE == std:("true")))) {
        console->info(std:("Skipping dependency installation in CI/test environment..."));
        return std::shared_ptr<Promise<void>>();
    }
    console->info(std:("Installing dependencies..."));
    std::async([=]() { runBunCommand(array<string>{ std:("install") }, targetDir); });
};


std::shared_ptr<Promise<void>> setupProjectEnvironment(string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    std::async([=]() { createProjectDirectories(targetDir); });
    auto envFilePath = string_empty + targetDir + std:("/.env");
    if (AND((database == std:("postgres")), (!isNonInteractive))) {
        std::async([=]() { promptAndStorePostgresUrl(envFilePath); });
    } else if (database == std:("pglite")) {
        std::async([=]() { setupPgLite(undefined, string_empty + targetDir + std:("/.env"), targetDir); });
    }
    std::async([=]() { setupAIModelConfig(aiModel, envFilePath, isNonInteractive); });
    if (embeddingModel) {
        std::async([=]() { setupEmbeddingModelConfig(embeddingModel, envFilePath, isNonInteractive); });
    }
    return std::shared_ptr<Promise<void>>();
};


