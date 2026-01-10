#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/config/pipeline.config.h"

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
