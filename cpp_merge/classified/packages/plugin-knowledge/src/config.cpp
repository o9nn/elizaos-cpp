#include "config.hpp"
#include <string>

std::shared_ptr<ModelConfig> validateModelConfig(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        shared getSetting = [=](auto key, auto defaultValue = std::nullopt) mutable
        {
            if (runtime) {
                return OR((OR((runtime->getSetting(key)), (const_(process->env)[key]))), (defaultValue));
            }
            return OR((const_(process->env)[key]), (defaultValue));
        };
        auto ctxKnowledgeEnabled = parseBooleanEnv(getSetting(std::string("CTX_KNOWLEDGE_ENABLED"), std::string("false")));
        logger->debug(std::string("[Document Processor] CTX_KNOWLEDGE_ENABLED: '") + ctxKnowledgeEnabled + std::string(" (runtime: ") + !!runtime + std::string(")"));
        auto embeddingProvider = getSetting(std::string("EMBEDDING_PROVIDER"));
        auto assumePluginOpenAI = !embeddingProvider;
        if (assumePluginOpenAI) {
            auto openaiApiKey = getSetting(std::string("OPENAI_API_KEY"));
            auto openaiEmbeddingModel = getSetting(std::string("OPENAI_EMBEDDING_MODEL"));
            if (AND((openaiApiKey), (openaiEmbeddingModel))) {
                logger->debug(std::string("[Document Processor] EMBEDDING_PROVIDER not specified, using configuration from plugin-openai"));
            } else {
                logger->debug(std::string("[Document Processor] EMBEDDING_PROVIDER not specified. Assuming embeddings are provided by another plugin (e.g., plugin-google-genai)."));
            }
        }
        auto finalEmbeddingProvider = embeddingProvider;
        auto textEmbeddingModel = (embeddingProvider == std::string("ollama")) ? OR((getSetting(std::string("OLLAMA_EMBEDDING_MODEL"))), (std::string("nomic-embed-text"))) : OR((OR((getSetting(std::string("TEXT_EMBEDDING_MODEL"))), (getSetting(std::string("OPENAI_EMBEDDING_MODEL"))))), (std::string("text-embedding-3-small")));
        auto embeddingDimension = OR((OR((getSetting(std::string("EMBEDDING_DIMENSION"))), (getSetting(std::string("OPENAI_EMBEDDING_DIMENSIONS"))))), (std::string("768")));
        auto openaiApiKey = getSetting(std::string("OPENAI_API_KEY"));
        auto config = ModelConfigSchema->parse(object{
            object::pair{std::string("EMBEDDING_PROVIDER"), finalEmbeddingProvider}, 
            object::pair{std::string("TEXT_PROVIDER"), getSetting(std::string("TEXT_PROVIDER"))}, 
            object::pair{std::string("OPENAI_API_KEY"), openaiApiKey}, 
            object::pair{std::string("ANTHROPIC_API_KEY"), getSetting(std::string("ANTHROPIC_API_KEY"))}, 
            object::pair{std::string("OPENROUTER_API_KEY"), getSetting(std::string("OPENROUTER_API_KEY"))}, 
            object::pair{std::string("GOOGLE_API_KEY"), getSetting(std::string("GOOGLE_API_KEY"))}, 
            object::pair{std::string("OLLAMA_API_KEY"), getSetting(std::string("OLLAMA_API_KEY"))}, 
            object::pair{std::string("OPENAI_BASE_URL"), getSetting(std::string("OPENAI_BASE_URL"))}, 
            object::pair{std::string("ANTHROPIC_BASE_URL"), getSetting(std::string("ANTHROPIC_BASE_URL"))}, 
            object::pair{std::string("OPENROUTER_BASE_URL"), getSetting(std::string("OPENROUTER_BASE_URL"))}, 
            object::pair{std::string("GOOGLE_BASE_URL"), getSetting(std::string("GOOGLE_BASE_URL"))}, 
            object::pair{std::string("OLLAMA_BASE_URL"), OR((OR((getSetting(std::string("OLLAMA_BASE_URL"))), (getSetting(std::string("OLLAMA_API_ENDPOINT"))["replace"](std::string("/api"), string_empty)))), (std::string("http://localhost:11434")))}, 
            object::pair{std::string("TEXT_EMBEDDING_MODEL"), textEmbeddingModel}, 
            object::pair{std::string("TEXT_MODEL"), getSetting(std::string("TEXT_MODEL"))}, 
            object::pair{std::string("MAX_INPUT_TOKENS"), getSetting(std::string("MAX_INPUT_TOKENS"), std::string("4000"))}, 
            object::pair{std::string("MAX_OUTPUT_TOKENS"), getSetting(std::string("MAX_OUTPUT_TOKENS"), std::string("4096"))}, 
            object::pair{std::string("EMBEDDING_DIMENSION"), embeddingDimension}, 
            object::pair{std::string("LOAD_DOCS_ON_STARTUP"), parseBooleanEnv(getSetting(std::string("LOAD_DOCS_ON_STARTUP")))}, 
            object::pair{std::string("CTX_KNOWLEDGE_ENABLED"), ctxKnowledgeEnabled}
        });
        validateConfigRequirements(config, assumePluginOpenAI);
        return config;
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            auto issues = error["issues"]["map"]([=](auto issue) mutable
            {
                return string_empty + issue["path"]->join(std::string(".")) + std::string(": ") + issue["message"] + string_empty;
            }
            )["join"](std::string(", "));
            throw any(std::make_shared<Error>(std::string("Model configuration validation failed: ") + issues + string_empty));
        }
        throw any(error);
    }
};


void validateConfigRequirements(std::shared_ptr<ModelConfig> config, boolean assumePluginOpenAI)
{
    auto embeddingProvider = config->EMBEDDING_PROVIDER;
    if (AND((embeddingProvider == std::string("openai")), (!config->OPENAI_API_KEY))) {
        throw any(std::make_shared<Error>(std::string("OPENAI_API_KEY is required when EMBEDDING_PROVIDER is set to "openai"")));
    }
    if (AND((embeddingProvider == std::string("google")), (!config->GOOGLE_API_KEY))) {
        throw any(std::make_shared<Error>(std::string("GOOGLE_API_KEY is required when EMBEDDING_PROVIDER is set to "google"")));
    }
    if (embeddingProvider == std::string("ollama")) {
        if (!config->OLLAMA_API_KEY) {
            logger->warn(std::string("OLLAMA_API_KEY not provided - using dummy key (this is often fine for Ollama)"));
        }
        logger->info(std::string("Ollama embedding uses model names (OLLAMA_EMBEDDING_MODEL) from @elizaos/plugin-ollama"));
    }
    if (!embeddingProvider) {
        logger->debug(std::string("[Document Processor] No EMBEDDING_PROVIDER specified. Embeddings will be handled by the runtime."));
    }
    if (AND((AND((assumePluginOpenAI), (config->OPENAI_API_KEY))), (!config->TEXT_EMBEDDING_MODEL))) {
        throw any(std::make_shared<Error>(std::string("OPENAI_EMBEDDING_MODEL is required when using plugin-openai configuration")));
    }
    if (config->CTX_KNOWLEDGE_ENABLED) {
        logger->debug(std::string("[Document Processor] CTX validation: Checking text generation settings..."));
        if (AND((config->TEXT_PROVIDER == std::string("openai")), (!config->OPENAI_API_KEY))) {
            throw any(std::make_shared<Error>(std::string("OPENAI_API_KEY is required when TEXT_PROVIDER is set to "openai"")));
        }
        if (AND((config->TEXT_PROVIDER == std::string("anthropic")), (!config->ANTHROPIC_API_KEY))) {
            throw any(std::make_shared<Error>(std::string("ANTHROPIC_API_KEY is required when TEXT_PROVIDER is set to "anthropic"")));
        }
        if (AND((config->TEXT_PROVIDER == std::string("openrouter")), (!config->OPENROUTER_API_KEY))) {
            throw any(std::make_shared<Error>(std::string("OPENROUTER_API_KEY is required when TEXT_PROVIDER is set to "openrouter"")));
        }
        if (AND((config->TEXT_PROVIDER == std::string("google")), (!config->GOOGLE_API_KEY))) {
            throw any(std::make_shared<Error>(std::string("GOOGLE_API_KEY is required when TEXT_PROVIDER is set to "google"")));
        }
        if (config->TEXT_PROVIDER == std::string("ollama")) {
            if (!config->OLLAMA_API_KEY) {
                logger->warn(std::string("OLLAMA_API_KEY not provided - using dummy key (this is often fine for Ollama)"));
            }
            if (AND((config->OLLAMA_BASE_URL), (config->OLLAMA_BASE_URL != std::string("http://localhost:11434")))) {
                logger->info(std::string("Using Ollama configuration: ") + config->OLLAMA_BASE_URL + string_empty);
            } else {
                logger->info(std::string("OLLAMA_BASE_URL not provided - using default http://localhost:11434"));
            }
        }
        if (config->TEXT_PROVIDER == std::string("openrouter")) {
            auto modelName = OR((config->TEXT_MODEL->toLowerCase()), (string_empty));
            if (OR((modelName["includes"](std::string("claude"))), (modelName["includes"](std::string("gemini"))))) {
                logger->debug(std::string("[Document Processor] Using ") + modelName + std::string(" with OpenRouter. This configuration supports document caching for improved performance."));
            }
        }
    } else {
        logger->info(std::string("[Document Processor] Contextual Knowledge is DISABLED!"));
        logger->info(std::string("[Document Processor] This means documents will NOT be enriched with context."));
        if (assumePluginOpenAI) {
            logger->info(std::string("[Document Processor] Embeddings will be handled by the runtime (e.g., plugin-openai, plugin-google-genai)."));
        } else {
            logger->info(std::string("[Document Processor] Using configured embedding provider for basic embeddings only."));
        }
    }
};


std::shared_ptr<Promise<std::shared_ptr<ProviderRateLimits>>> getProviderRateLimits(std::shared_ptr<IAgentRuntime> runtime)
{
    auto config = validateModelConfig(runtime);
    shared getSetting = [=](auto key, auto defaultValue) mutable
    {
        if (runtime) {
            return OR((runtime->getSetting(key)), (defaultValue));
        }
        return OR((const_(process->env)[key]), (defaultValue));
    };
    auto maxConcurrentRequests = parseInt(getSetting(std::string("MAX_CONCURRENT_REQUESTS"), std::string("30")), 10);
    auto requestsPerMinute = parseInt(getSetting(std::string("REQUESTS_PER_MINUTE"), std::string("60")), 10);
    auto tokensPerMinute = parseInt(getSetting(std::string("TOKENS_PER_MINUTE"), std::string("150000")), 10);
    auto primaryProvider = OR((config->TEXT_PROVIDER), (config->EMBEDDING_PROVIDER));
    logger->debug(std::string("[Document Processor] Rate limiting for ") + primaryProvider + std::string(": ") + requestsPerMinute + std::string(" RPM, ") + tokensPerMinute + std::string(" TPM, ") + maxConcurrentRequests + std::string(" concurrent"));
    static switch_type __switch10442_11625 = {
        { any(std::string("anthropic")), 1 },
        { any(std::string("openai")), 2 },
        { any(std::string("google")), 3 }
    };
    switch (__switch10442_11625[primaryProvider])
    {
    case 1:
        return object{
            object::pair{std::string("maxConcurrentRequests"), std::string("maxConcurrentRequests")}, 
            object::pair{std::string("requestsPerMinute"), std::string("requestsPerMinute")}, 
            object::pair{std::string("tokensPerMinute"), std::string("tokensPerMinute")}, 
            object::pair{std::string("provider"), std::string("anthropic")}
        };
    case 2:
        return object{
            object::pair{std::string("maxConcurrentRequests"), std::string("maxConcurrentRequests")}, 
            object::pair{std::string("requestsPerMinute"), Math->min(requestsPerMinute, 3000)}, 
            object::pair{std::string("tokensPerMinute"), Math->min(tokensPerMinute, 150000)}, 
            object::pair{std::string("provider"), std::string("openai")}
        };
    case 3:
        return object{
            object::pair{std::string("maxConcurrentRequests"), std::string("maxConcurrentRequests")}, 
            object::pair{std::string("requestsPerMinute"), Math->min(requestsPerMinute, 60)}, 
            object::pair{std::string("tokensPerMinute"), Math->min(tokensPerMinute, 100000)}, 
            object::pair{std::string("provider"), std::string("google")}
        };
    default:
        return object{
            object::pair{std::string("maxConcurrentRequests"), std::string("maxConcurrentRequests")}, 
            object::pair{std::string("requestsPerMinute"), std::string("requestsPerMinute")}, 
            object::pair{std::string("tokensPerMinute"), std::string("tokensPerMinute")}, 
            object::pair{std::string("provider"), OR((primaryProvider), (std::string("unknown")))}
        };
    }
};


std::function<boolean(any)> parseBooleanEnv = [=](auto value) mutable
{
    if (type_of(value) == std::string("boolean")) return value;
    if (type_of(value) == std::string("string")) return value->toLowerCase() == std::string("true");
    return false;
};

void Main(void)
{
}

MAIN
