#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-knowledge/src/types.h"

namespace @elizaos/core {
}
any ModelConfigSchema = z->object(object{
    object::pair{std::string("EMBEDDING_PROVIDER"), z->enum(array<string>{ std::string("openai"), std::string("google"), std::string("ollama") })->optional()}, 
    object::pair{std::string("TEXT_PROVIDER"), z->enum(array<string>{ std::string("openai"), std::string("anthropic"), std::string("openrouter"), std::string("google"), std::string("ollama") })->optional()}, 
    object::pair{std::string("OPENAI_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("ANTHROPIC_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("OPENROUTER_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("GOOGLE_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("OLLAMA_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("OPENAI_BASE_URL"), z->string()->optional()}, 
    object::pair{std::string("ANTHROPIC_BASE_URL"), z->string()->optional()}, 
    object::pair{std::string("OPENROUTER_BASE_URL"), z->string()->optional()}, 
    object::pair{std::string("GOOGLE_BASE_URL"), z->string()->optional()}, 
    object::pair{std::string("OLLAMA_BASE_URL"), z->string()->optional()}, 
    object::pair{std::string("OLLAMA_API_ENDPOINT"), z->string()->optional()}, 
    object::pair{std::string("TEXT_EMBEDDING_MODEL"), z->string()}, 
    object::pair{std::string("TEXT_MODEL"), z->string()->optional()}, 
    object::pair{std::string("MAX_INPUT_TOKENS"), z->string()->or(z->number())->transform([=](auto val) mutable
    {
        return ((type_of(val) == std::string("string")) ? any(parseInt(val, 10)) : any(val));
    }
    )}, 
    object::pair{std::string("MAX_OUTPUT_TOKENS"), z->string()->or(z->number())->optional()->transform([=](auto val) mutable
    {
        return ((val) ? any(((type_of(val) == std::string("string")) ? any(parseInt(val, 10)) : any(val))) : any(4096));
    }
    )}, 
    object::pair{std::string("EMBEDDING_DIMENSION"), z->string()->or(z->number())->optional()->transform([=](auto val) mutable
    {
        return ((val) ? any(((type_of(val) == std::string("string")) ? any(parseInt(val, 10)) : any(val))) : any(768));
    }
    )}, 
    object::pair{std::string("LOAD_DOCS_ON_STARTUP"), z->boolean()->default(false)}, 
    object::pair{std::string("CTX_KNOWLEDGE_ENABLED"), z->boolean()->default(false)}
});
object KnowledgeServiceType = object{
    object::pair{std::string("KNOWLEDGE"), as<std::shared_ptr<const>>(std::string("knowledge"))}
}, satisfies, Partial;

void Main(void)
{
    static_cast<any>();
}

MAIN
