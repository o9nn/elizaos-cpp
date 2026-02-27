#include "config.hpp"

std::shared_ptr<ModelConfig> validateModelConfig(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        shared getSetting = [=](auto key, auto defaultValue = undefined) mutable
        {
            if (runtime) {
                return OR((OR((runtime->getSetting(key)), (const_(process->env)[key]))), (defaultValue));
            }
            return OR((const_(process->env)[key]), (defaultValue));
        };
        auto ctxKnowledgeEnabled = parseBooleanEnv(getSetting(std:("CTX_KNOWLEDGE_ENABLED"), std:("false")));
        logger->debug(std:("[Document Processor] CTX_KNOWLEDGE_ENABLED: '") + ctxKnowledgeEnabled + std:(" (runtime: ") + !!runtime + std:(")"));
        auto embeddingProvider = getSetting(std:("EMBEDDING_PROVIDER"));
        auto assumePluginOpenAI = !embeddingProvider;
        if (assumePluginOpenAI) {
            auto openaiApiKey = getSetting(std:("OPENAI_API_KEY"));
            auto openaiEmbeddingModel = getSetting(std:("OPENAI_EMBEDDING_MODEL"));
            if (AND((openaiApiKey), (openaiEmbeddingModel))) {
                logger->debug(std:("[Document Processor] EMBEDDING_PROVIDER not specified, using configuration from plugin-openai"));
            } else {
                logger->debug(std:("[Document Processor] EMBEDDING_PROVIDER not specified. Assuming embeddings are provided by another plugin (e.g., plugin-google-genai)."));
            }
        }
        auto finalEmbeddingProvider = embeddingProvider;
        auto textEmbeddingModel = (embeddingProvider == std:("ollama")) ? OR((getSetting(std:("OLLAMA_EMBEDDING_MODEL"))), (std:("nomic-embed-text"))) : OR((OR((getSetting(std:("TEXT_EMBEDDING_MODEL"))), (getSetting(std:("OPENAI_EMBEDDING_MODEL"))))), (std:("text-embedding-3-small")));
        auto embeddingDimension = OR((OR((getSetting(std:("EMBEDDING_DIMENSION"))), (getSetting(std:("OPENAI_EMBEDDING_DIMENSIONS"))))), (std:("768")));
        auto openaiApiKey = getSetting(std:("OPENAI_API_KEY"));
        auto config = ModelConfigSchema->parse(object{
            object::pair{std:("EMBEDDING_PROVIDER"), finalEmbeddingProvider}, 
            object::pair{std:("TEXT_PROVIDER"), getSetting(std:("TEXT_PROVIDER"))}, 
            object::pair{std:("OPENAI_API_KEY"), openaiApiKey}, 
            object::pair{std:("ANTHROPIC_API_KEY"), getSetting(std:("ANTHROPIC_API_KEY"))}, 
            object::pair{std:("OPENROUTER_API_KEY"), getSetting(std:("OPENROUTER_API_KEY"))}, 
            object::pair{std:("GOOGLE_API_KEY"), getSetting(std:("GOOGLE_API_KEY"))}, 
            object::pair{std:("OLLAMA_API_KEY"), getSetting(std:("OLLAMA_API_KEY"))}, 
            object::pair{std:("OPENAI_BASE_URL"), getSetting(std:("OPENAI_BASE_URL"))}, 
            object::pair{std:("ANTHROPIC_BASE_URL"), getSetting(std:("ANTHROPIC_BASE_URL"))}, 
            object::pair{std:("OPENROUTER_BASE_URL"), getSetting(std:("OPENROUTER_BASE_URL"))}, 
            object::pair{std:("GOOGLE_BASE_URL"), getSetting(std:("GOOGLE_BASE_URL"))}, 
            object::pair{std:("OLLAMA_BASE_URL"), OR((OR((getSetting(std:("OLLAMA_BASE_URL"))), (getSetting(std:("OLLAMA_API_ENDPOINT"))["replace"](std:("/api"), string_empty)))), (std:("http://localhost:11434")))}, 
            object::pair{std:("TEXT_EMBEDDING_MODEL"), textEmbeddingModel}, 
            object::pair{std:("TEXT_MODEL"), getSetting(std:("TEXT_MODEL"))}, 
            object::pair{std:("MAX_INPUT_TOKENS"), getSetting(std:("MAX_INPUT_TOKENS"), std:("4000"))}, 
            object::pair{std:("MAX_OUTPUT_TOKENS"), getSetting(std:("MAX_OUTPUT_TOKENS"), std:("4096"))}, 
            object::pair{std:("EMBEDDING_DIMENSION"), embeddingDimension}, 
            object::pair{std:("LOAD_DOCS_ON_STARTUP"), parseBooleanEnv(getSetting(std:("LOAD_DOCS_ON_STARTUP")))}, 
            object::pair{std:("CTX_KNOWLEDGE_ENABLED"), ctxKnowledgeEnabled}
        });
        validateConfigRequirements(config, assumePluginOpenAI);
        return config;
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            auto issues = error["issues"]["map"]([=](auto issue) mutable
            {
                return string_empty + issue["path"]->join(std:(".")) + std:(": ") + issue["message"] + string_empty;
            }
            )["join"](std:(", "));
            throw any(std::make_shared<Error>(std:("Model configuration validation failed: ") + issues + string_empty));
        }
        throw any(error);
    }
};


void validateConfigRequirements(std::shared_ptr<ModelConfig> config, boolean assumePluginOpenAI)
{
    auto embeddingProvider = config->EMBEDDING_PROVIDER;
    if (AND((embeddingProvider == std:("openai")), (!config->OPENAI_API_KEY))) {
        throw any(std::make_shared<Error>(std:("OPENAI_API_KEY is required when EMBEDDING_PROVIDER is set to "openai"")));
    }
    if (AND((embeddingProvider == std:("google")), (!config->GOOGLE_API_KEY))) {
        throw any(std::make_shared<Error>(std:("GOOGLE_API_KEY is required when EMBEDDING_PROVIDER is set to "google"")));
    }
    if (embeddingProvider == std:("ollama")) {
        if (!config->OLLAMA_API_KEY) {
            logger->warn(std:("OLLAMA_API_KEY not provided - using dummy key (this is often fine for Ollama)"));
        }
        logger->info(std:("Ollama embedding uses model names (OLLAMA_EMBEDDING_MODEL) from @elizaos/plugin-ollama"));
    }
    if (!embeddingProvider) {
        logger->debug(std:("[Document Processor] No EMBEDDING_PROVIDER specified. Embeddings will be handled by the runtime."));
    }
    if (AND((AND((assumePluginOpenAI), (config->OPENAI_API_KEY))), (!config->TEXT_EMBEDDING_MODEL))) {
        throw any(std::make_shared<Error>(std:("OPENAI_EMBEDDING_MODEL is required when using plugin-openai configuration")));
    }
    if (config->CTX_KNOWLEDGE_ENABLED) {
        logger->debug(std:("[Document Processor] CTX validation: Checking text generation settings..."));
        if (AND((config->TEXT_PROVIDER == std:("openai")), (!config->OPENAI_API_KEY))) {
            throw any(std::make_shared<Error>(std:("OPENAI_API_KEY is required when TEXT_PROVIDER is set to "openai"")));
        }
        if (AND((config->TEXT_PROVIDER == std:("anthropic")), (!config->ANTHROPIC_API_KEY))) {
            throw any(std::make_shared<Error>(std:("ANTHROPIC_API_KEY is required when TEXT_PROVIDER is set to "anthropic"")));
        }
        if (AND((config->TEXT_PROVIDER == std:("openrouter")), (!config->OPENROUTER_API_KEY))) {
            throw any(std::make_shared<Error>(std:("OPENROUTER_API_KEY is required when TEXT_PROVIDER is set to "openrouter"")));
        }
        if (AND((config->TEXT_PROVIDER == std:("google")), (!config->GOOGLE_API_KEY))) {
            throw any(std::make_shared<Error>(std:("GOOGLE_API_KEY is required when TEXT_PROVIDER is set to "google"")));
        }
        if (config->TEXT_PROVIDER == std:("ollama")) {
            if (!config->OLLAMA_API_KEY) {
                logger->warn(std:("OLLAMA_API_KEY not provided - using dummy key (this is often fine for Ollama)"));
            }
            if (AND((config->OLLAMA_BASE_URL), (config->OLLAMA_BASE_URL != std:("http://localhost:11434")))) {
                logger->info(std:("Using Ollama configuration: ") + config->OLLAMA_BASE_URL + string_empty);
            } else {
                logger->info(std:("OLLAMA_BASE_URL not provided - using default http://localhost:11434"));
            }
        }
        if (config->TEXT_PROVIDER == std:("openrouter")) {
            auto modelName = OR((config->TEXT_MODEL->toLowerCase()), (string_empty));
            if (OR((modelName["includes"](std:("claude"))), (modelName["includes"](std:("gemini"))))) {
                logger->debug(std:("[Document Processor] Using ") + modelName + std:(" with OpenRouter. This configuration supports document caching for improved performance."));
            }
        }
    } else {
        logger->info(std:("[Document Processor] Contextual Knowledge is DISABLED!"));
        logger->info(std:("[Document Processor] This means documents will NOT be enriched with context."));
        if (assumePluginOpenAI) {
            logger->info(std:("[Document Processor] Embeddings will be handled by the runtime (e.g., plugin-openai, plugin-google-genai)."));
        } else {
            logger->info(std:("[Document Processor] Using configured embedding provider for basic embeddings only."));
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
    auto maxConcurrentRequests = parseInt(getSetting(std:("MAX_CONCURRENT_REQUESTS"), std:("30")), 10);
    auto requestsPerMinute = parseInt(getSetting(std:("REQUESTS_PER_MINUTE"), std:("60")), 10);
    auto tokensPerMinute = parseInt(getSetting(std:("TOKENS_PER_MINUTE"), std:("150000")), 10);
    auto primaryProvider = OR((config->TEXT_PROVIDER), (config->EMBEDDING_PROVIDER));
    logger->debug(std:("[Document Processor] Rate limiting for ") + primaryProvider + std:(": ") + requestsPerMinute + std:(" RPM, ") + tokensPerMinute + std:(" TPM, ") + maxConcurrentRequests + std:(" concurrent"));
    static switch_type __switch10442_11625 = {
        { any(std:("anthropic")), 1 },
        { any(std:("openai")), 2 },
        { any(std:("google")), 3 }
    };
    switch (__switch10442_11625[primaryProvider])
    {
    case 1:
        return object{
            object::pair{std:("maxConcurrentRequests"), std:("maxConcurrentRequests")}, 
            object::pair{std:("requestsPerMinute"), std:("requestsPerMinute")}, 
            object::pair{std:("tokensPerMinute"), std:("tokensPerMinute")}, 
            object::pair{std:("provider"), std:("anthropic")}
        };
    case 2:
        return object{
            object::pair{std:("maxConcurrentRequests"), std:("maxConcurrentRequests")}, 
            object::pair{std:("requestsPerMinute"), Math->min(requestsPerMinute, 3000)}, 
            object::pair{std:("tokensPerMinute"), Math->min(tokensPerMinute, 150000)}, 
            object::pair{std:("provider"), std:("openai")}
        };
    case 3:
        return object{
            object::pair{std:("maxConcurrentRequests"), std:("maxConcurrentRequests")}, 
            object::pair{std:("requestsPerMinute"), Math->min(requestsPerMinute, 60)}, 
            object::pair{std:("tokensPerMinute"), Math->min(tokensPerMinute, 100000)}, 
            object::pair{std:("provider"), std:("google")}
        };
    default:
        return object{
            object::pair{std:("maxConcurrentRequests"), std:("maxConcurrentRequests")}, 
            object::pair{std:("requestsPerMinute"), std:("requestsPerMinute")}, 
            object::pair{std:("tokensPerMinute"), std:("tokensPerMinute")}, 
            object::pair{std:("provider"), OR((primaryProvider), (std:("unknown")))}
        };
    }
};


std::function<boolean(any)> parseBooleanEnv = [=](auto value) mutable
{
    if (type_of(value) == std:("boolean")) return value;
    if (type_of(value) == std:("string")) return value->toLowerCase() == std:("true");
    return false;
};

void Main(void)
{
}

MAIN
