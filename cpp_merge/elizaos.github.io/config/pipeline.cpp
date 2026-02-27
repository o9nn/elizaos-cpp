#include "pipeline.config.h"

string openrouterApiKey = process->env->OPENROUTER_API_KEY;

void Main(void)
{
    if (!openrouterApiKey) {
        console->warn(std:("OPENROUTER_API_KEY is not set"));
    }
    satisfies;
    PipelineConfig;
}

MAIN
