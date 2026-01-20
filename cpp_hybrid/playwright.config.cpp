#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/playwright.config.h"

double PORT = (process->env->TEST_PORT) ? any(parseInt(process->env->TEST_PORT)) : any(4444);
string BASE_URL = std::string("http://localhost:") + PORT + string_empty;

void Main(void)
{
}

MAIN
