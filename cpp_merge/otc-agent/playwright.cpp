#include "playwright.config.h"
#include <string>

double PORT = (process->env->TEST_PORT) ? any(parseInt(process->env->TEST_PORT)) (4444);
string BASE_URL = std::string("http://localhost:") + PORT + string_empty;

void Main(void)
{
}

MAIN
