#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/summarize/config.h"

any AISummaryConfigSchema = z->object(object{
    object::pair{std::string("enabled"), z->boolean()->default(false)}, 
    object::pair{std::string("defaultModel"), z->string()}, 
    object::pair{std::string("models"), z->object(object{
        object::pair{std::string("day"), z->string()}, 
        object::pair{std::string("week"), z->string()}, 
        object::pair{std::string("month"), z->string()}
    })}, 
    object::pair{std::string("apiKey"), z->string()}, 
    object::pair{std::string("endpoint"), z->string()->default(std::string("https://openrouter.ai/api/v1/chat/completions"))}, 
    object::pair{std::string("temperature"), z->number()->default(0.1)}, 
    object::pair{std::string("max_tokens"), z->number()->default(200)}, 
    object::pair{std::string("projectContext"), z->string()->default(std::string("An open source project on GitHub."))}
});

void Main(void)
{
}

MAIN
