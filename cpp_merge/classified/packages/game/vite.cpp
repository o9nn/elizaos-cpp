#include "vite.config.h"
#include <string>

double FRONTEND_PORT = parseInt(OR((process->env->FRONTEND_PORT), (std::string("5173"))), 10);

void Main(void)
{
}

MAIN
