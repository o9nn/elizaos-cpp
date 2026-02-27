#include "types.hpp"

namespace @elizaos/core {
}
any ModelConfigSchema = z->object(object{
    object::pair{std:("EMBEDDING_PROVIDER"), z->enum(array<string>{ std:("openai"), std:("google"), std:("ollama") })->optional()}, 
    object::pair{std:("TEXT_PROVIDER"), z->enum(array<string>{ std:("openai"), std:("anthropic"), std:("openrouter"), std:("google"), std:("ollama") })->optional()}, 
    object::pair{std:("OPENAI_API_KEY"), z->string()->optional()}, 
    object::pair{std:("ANTHROPIC_API_KEY"), z->string()->optional()}, 
    object::pair{std:("OPENROUTER_API_KEY"), z->string()->optional()}, 
    object::pair{std:("GOOGLE_API_KEY"), z->string()->optional()}, 
    object::pair{std:("OLLAMA_API_KEY"), z->string()->optional()}, 
    object::pair{std:("OPENAI_BASE_URL"), z->string()->optional()}, 
    object::pair{std:("ANTHROPIC_BASE_URL"), z->string()->optional()}, 
    object::pair{std:("OPENROUTER_BASE_URL"), z->string()->optional()}, 
    object::pair{std:("GOOGLE_BASE_URL"), z->string()->optional()}, 
    object::pair{std:("OLLAMA_BASE_URL"), z->string()->optional()}, 
    object::pair{std:("OLLAMA_API_ENDPOINT"), z->string()->optional()}, 
    object::pair{std:("TEXT_EMBEDDING_MODEL"), z->string()}, 
    object::pair{std:("TEXT_MODEL"), z->string()->optional()}, 
    object::pair{std:("MAX_INPUT_TOKENS"), z->string()->or(z->number())->transform([=](auto val) mutable
    {
        return ((type_of(val) == std:("string")) ? any(parseInt(val, 10)) (val));
    }
    )}, 
    object::pair{std:("MAX_OUTPUT_TOKENS"), z->string()->or(z->number())->optional()->transform([=](auto val) mutable
    {
        return ((val) ? any(((type_of(val) == std:("string")) ? any(parseInt(val, 10)) (val))) (4096));
    }
    )}, 
    object::pair{std:("EMBEDDING_DIMENSION"), z->string()->or(z->number())->optional()->transform([=](auto val) mutable
    {
        return ((val) ? any(((type_of(val) == std:("string")) ? any(parseInt(val, 10)) (val))) (768));
    }
    )}, 
    object::pair{std:("LOAD_DOCS_ON_STARTUP"), z->boolean()->default(false)}, 
    object::pair{std:("CTX_KNOWLEDGE_ENABLED"), z->boolean()->default(false)}
});
object KnowledgeServiceType = object{
    object::pair{std:("KNOWLEDGE"), as<std::shared_ptr<const>>(std:("knowledge"))}
}, satisfies, Partial;

void Main(void)
{
    static_cast<any>();
}

MAIN
