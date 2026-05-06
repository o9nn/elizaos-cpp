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
            { any(std::string("local")), 1 },
            { any(std::string("openai")), 2 },
            { any(std::string("claude")), 3 },
            { any(std::string("openrouter")), 4 },
            { any(std::string("ollama")), 5 },
            { any(std::string("google")), 6 }
        };
        switch (__switch770_5802[aiModel])
        {
        case 1:
            {
                console->info(std::string("[√] Using Local AI - no additional configuration needed"));
                break;
            }
        case 2:
            {
                if (isNonInteractive) {
                    auto content = string_empty;
                    if (existsSync(envFilePath)) {
                        content = std::async([=]() { fs->readFile(envFilePath, std::string("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std::string("\
"))))) {
                        content += std::string("\
");
                    }
                    content += std::string("\
# AI Model Configuration\
");
                    content += std::string("# OpenAI Configuration\
");
                    content += std::string("OPENAI_API_KEY=your_openai_api_key_here\
");
                    content += std::string("# Get your API key from: https://platform.openai.com/api-keys\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                    console->info(std::string("[√] OpenAI placeholder configuration added to .env file"));
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
                        content = std::async([=]() { fs->readFile(envFilePath, std::string("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std::string("\
"))))) {
                        content += std::string("\
");
                    }
                    content += std::string("\
# AI Model Configuration\
");
                    content += std::string("# Anthropic API Configuration\
");
                    content += std::string("ANTHROPIC_API_KEY=your_anthropic_api_key_here\
");
                    content += std::string("# Get your API key from: https://console.anthropic.com/\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                    console->info(std::string("[√] Anthropic API placeholder configuration added to .env file"));
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
                        content = std::async([=]() { fs->readFile(envFilePath, std::string("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std::string("\
"))))) {
                        content += std::string("\
");
                    }
                    content += std::string("\
# AI Model Configuration\
");
                    content += std::string("# OpenRouter Configuration\
");
                    content += std::string("OPENROUTER_API_KEY=your_openrouter_api_key_here\
");
                    content += std::string("# Get your API key from: https://openrouter.ai/keys\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                    console->info(std::string("[√] OpenRouter placeholder configuration added to .env file"));
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
                        content = std::async([=]() { fs->readFile(envFilePath, std::string("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std::string("\
"))))) {
                        content += std::string("\
");
                    }
                    content += std::string("\
# AI Model Configuration\
");
                    content += std::string("# Ollama Configuration\
");
                    content += std::string("OLLAMA_API_ENDPOINT=http://localhost:11434\
");
                    content += std::string("OLLAMA_MODEL=llama2\
");
                    content += std::string("USE_OLLAMA_TEXT_MODELS=true\
");
                    content += std::string("# Make sure Ollama is installed and running: https://ollama.ai/\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                    console->info(std::string("[√] Ollama placeholder configuration added to .env file"));
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
                        content = std::async([=]() { fs->readFile(envFilePath, std::string("utf8")); });
                    }
                    if (AND((content), (!content->endsWith(std::string("\
"))))) {
                        content += std::string("\
");
                    }
                    content += std::string("\
# AI Model Configuration\
");
                    content += std::string("# Google Generative AI Configuration\
");
                    content += std::string("GOOGLE_GENERATIVE_AI_API_KEY=your_google_api_key_here\
");
                    content += std::string("# Get your API key from: https://aistudio.google.com/apikey\
");
                    std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                    console->info(std::string("[√] Google Generative AI placeholder configuration added to .env file"));
                } else {
                    std::async([=]() { promptAndStoreGoogleKey(envFilePath); });
                }
                break;
            }
        default:
            console->warn(std::string("Unknown AI model: ") + aiModel + std::string(", skipping configuration"));
            return std::shared_ptr<Promise<void>>();
        }
    }
    catch (const any& error)
    {
        auto errorMessage = (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"));
        console->error(std::string("Failed to set up AI model configuration: ") + errorMessage + string_empty);
    }
};


boolean hasValidApiKey(string content, string keyName)
{
    auto regex = std::make_shared<RegExp>(std::string("^") + keyName + std::string("=(.+)$"), std::string("m"));
    auto match = content->match(regex);
    if (!match) return false;
    auto value = (*const_(match))[1]->trim();
    return (AND((AND((AND((AND((value != string_empty), (!value->includes(std::string("your_"))))), (!value->includes(std::string("_here"))))), (!value->includes(std::string("PLACEHOLDER"))))), (!value->includes(std::string("placeholder")))));
};


std::shared_ptr<Promise<void>> setupEmbeddingModelConfig(string embeddingModel, string envFilePath, boolean isNonInteractive)
{
    try
    {
        auto content = string_empty;
        if (existsSync(envFilePath)) {
            content = std::async([=]() { fs->readFile(envFilePath, std::string("utf8")); });
        }
        if (AND((content), (!content->endsWith(std::string("\
"))))) {
            content += std::string("\
");
        }
        static switch_type __switch6997_11974 = {
            { any(std::string("local")), 1 },
            { any(std::string("openai")), 2 },
            { any(std::string("ollama")), 3 },
            { any(std::string("google")), 4 }
        };
        switch (__switch6997_11974[embeddingModel])
        {
        case 1:
            {
                content += std::string("\
# Embedding Model Configuration (Fallback)\
");
                content += std::string("# Using local embeddings - no additional configuration needed\
");
                std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                console->info(std::string("[√] Using Local embeddings - no additional configuration needed"));
                break;
            }
        case 2:
            {
                if (!hasValidApiKey(content, std::string("OPENAI_API_KEY"))) {
                    if (isNonInteractive) {
                        if (!content->includes(std::string("OPENAI_API_KEY="))) {
                            content += std::string("\
# Embedding Model Configuration (Fallback)\
");
                            content += std::string("# OpenAI Embeddings Configuration\
");
                            content += std::string("OPENAI_API_KEY=your_openai_api_key_here\
");
                            content += std::string("# Get your API key from: https://platform.openai.com/api-keys\
");
                        }
                        std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                        console->info(std::string("[√] OpenAI embeddings placeholder configuration added to .env file"));
                    } else {
                        console->info(std::string("\
[!] OpenAI API key is required for embeddings"));
                        std::async([=]() { promptAndStoreOpenAIKey(envFilePath); });
                    }
                } else {
                    console->info(std::string("[√] OpenAI API key already configured - will use for embeddings"));
                }
                break;
            }
        case 3:
            {
                if (!hasValidApiKey(content, std::string("OLLAMA_API_ENDPOINT"))) {
                    if (isNonInteractive) {
                        if (!content->includes(std::string("OLLAMA_API_ENDPOINT="))) {
                            content += std::string("\
# Embedding Model Configuration (Fallback)\
");
                            content += std::string("# Ollama Embeddings Configuration\
");
                            content += std::string("OLLAMA_API_ENDPOINT=http://localhost:11434\
");
                            content += std::string("OLLAMA_EMBEDDING_MODEL=nomic-embed-text\
");
                            content += std::string("USE_OLLAMA_EMBEDDINGS=true\
");
                            content += std::string("# Make sure Ollama is installed and running: https://ollama.ai/\
");
                        }
                        std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                        console->info(std::string("[√] Ollama embeddings placeholder configuration added to .env file"));
                    } else {
                        console->info(std::string("\
[!] Ollama embedding model configuration is required"));
                        std::async([=]() { promptAndStoreOllamaEmbeddingConfig(envFilePath); });
                    }
                } else {
                    if (isNonInteractive) {
                        if (!content->includes(std::string("OLLAMA_EMBEDDING_MODEL"))) {
                            content += std::string("OLLAMA_EMBEDDING_MODEL=nomic-embed-text\
");
                        }
                        if (!content->includes(std::string("USE_OLLAMA_EMBEDDINGS"))) {
                            content += std::string("USE_OLLAMA_EMBEDDINGS=true\
");
                        }
                        std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                        console->info(std::string("[√] Ollama embedding model configuration added to .env file"));
                    } else {
                        console->info(std::string("\
[!] Please select an Ollama embedding model"));
                        std::async([=]() { promptAndStoreOllamaEmbeddingConfig(envFilePath); });
                    }
                }
                break;
            }
        case 4:
            {
                if (!hasValidApiKey(content, std::string("GOOGLE_GENERATIVE_AI_API_KEY"))) {
                    if (isNonInteractive) {
                        if (!content->includes(std::string("GOOGLE_GENERATIVE_AI_API_KEY="))) {
                            content += std::string("\
# Embedding Model Configuration (Fallback)\
");
                            content += std::string("# Google Generative AI Embeddings Configuration\
");
                            content += std::string("GOOGLE_GENERATIVE_AI_API_KEY=your_google_api_key_here\
");
                            content += std::string("# Get your API key from: https://aistudio.google.com/apikey\
");
                        }
                        std::async([=]() { fs->writeFile(envFilePath, content, std::string("utf8")); });
                        console->info(std::string("[√] Google embeddings placeholder configuration added to .env file"));
                    } else {
                        console->info(std::string("\
[!] Google Generative AI API key is required for embeddings"));
                        std::async([=]() { promptAndStoreGoogleKey(envFilePath); });
                    }
                } else {
                    console->info(std::string("[√] Google API key already configured - will use for embeddings"));
                }
                break;
            }
        default:
            console->warn(std::string("Unknown embedding model: ") + embeddingModel + std::string(", skipping configuration"));
            return std::shared_ptr<Promise<void>>();
        }
    }
    catch (const any& error)
    {
        auto errorMessage = (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"));
        console->error(std::string("Failed to set up embedding model configuration: ") + errorMessage + string_empty);
    }
};


std::shared_ptr<Promise<void>> installDependencies(string targetDir)
{
    if (OR((process->env->CI == std::string("true")), (process->env->ELIZA_TEST_MODE == std::string("true")))) {
        console->info(std::string("Skipping dependency installation in CI/test environment..."));
        return std::shared_ptr<Promise<void>>();
    }
    console->info(std::string("Installing dependencies..."));
    std::async([=]() { runBunCommand(array<string>{ std::string("install") }, targetDir); });
};


std::shared_ptr<Promise<void>> setupProjectEnvironment(string targetDir, string database, string aiModel, string embeddingModel, boolean isNonInteractive)
{
    std::async([=]() { createProjectDirectories(targetDir); });
    auto envFilePath = string_empty + targetDir + std::string("/.env");
    if (AND((database == std::string("postgres")), (!isNonInteractive))) {
        std::async([=]() { promptAndStorePostgresUrl(envFilePath); });
    } else if (database == std::string("pglite")) {
        std::async([=]() { setupPgLite(undefined, string_empty + targetDir + std::string("/.env"), targetDir); });
    }
    std::async([=]() { setupAIModelConfig(aiModel, envFilePath, isNonInteractive); });
    if (embeddingModel) {
        std::async([=]() { setupEmbeddingModelConfig(embeddingModel, envFilePath, isNonInteractive); });
    }
    return std::shared_ptr<Promise<void>>();
};


