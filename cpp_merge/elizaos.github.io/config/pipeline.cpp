#include "pipeline.config.h"
#include <string>

string openrouterApiKey = process->env->OPENROUTER_API_KEY;

void Main(void)
{
    if (!openrouterApiKey) {
        console->warn(std::string("OPENROUTER_API_KEY is not set"));
    }
    satisfies;
    PipelineConfig;
}

MAIN
