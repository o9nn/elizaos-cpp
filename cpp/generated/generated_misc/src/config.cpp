#include "elizaos.github.io/src/lib/pipelines/summarize/config.h"

std::any AISummaryConfigSchema = z->object(object{
    object::pair{std::string("enabled"), z->boolean()->default(false)}, 
    object::pair{std::string("defaultModel"), z->std::string()}, 
    object::pair{std::string("models"), z->object(object{
        object::pair{std::string("day"), z->std::string()}, 
        object::pair{std::string("week"), z->std::string()}, 
        object::pair{std::string("month"), z->std::string()}
    })}, 
    object::pair{std::string("apiKey"), z->std::string()}, 
    object::pair{std::string("endpoint"), z->std::string()->default(std::string("https://openrouter.ai/api/v1/chat/completions"))}, 
    object::pair{std::string("temperature"), z->number()->default(0.1)}, 
    object::pair{std::string("max_tokens"), z->number()->default(200)}, 
    object::pair{std::string("projectContext"), z->std::string()->default(std::string("An open source project on GitHub."))}
});

void Main(void)
{
}

MAIN
