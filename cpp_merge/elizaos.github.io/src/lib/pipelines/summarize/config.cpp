#include "config.hpp"

any AISummaryConfigSchema = z->object(object{
    object::pair{std:("enabled"), z->boolean()->default(false)}, 
    object::pair{std:("defaultModel"), z->string()}, 
    object::pair{std:("models"), z->object(object{
        object::pair{std:("day"), z->string()}, 
        object::pair{std:("week"), z->string()}, 
        object::pair{std:("month"), z->string()}
    })}, 
    object::pair{std:("apiKey"), z->string()}, 
    object::pair{std:("endpoint"), z->string()->default(std:("https://openrouter.ai/api/v1/chat/completions"))}, 
    object::pair{std:("temperature"), z->number()->default(0.1)}, 
    object::pair{std:("max_tokens"), z->number()->default(200)}, 
    object::pair{std:("projectContext"), z->string()->default(std:("An open source project on GitHub."))}
});

void Main(void)
{
}

MAIN
