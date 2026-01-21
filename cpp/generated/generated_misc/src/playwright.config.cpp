#include "otc-agent/playwright.config.h"

double PORT = (process->env->TEST_PORT) ? std::any(parseInt(process->env->TEST_PORT)) : std::any(4444);
std::string BASE_URL = std::string("http://localhost:") + PORT + string_empty;

void Main(void)
{
}

MAIN
