#include "otc-agent/synpress.config.h"

double OTC_DESK_PORT = parseInt(OR((process->env->OTC_DESK_PORT), (std::string("4444"))));
std::string BASE_URL = std::string("http://localhost:") + OTC_DESK_PORT + string_empty;

void Main(void)
{
}

MAIN
