#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/synpress.config.h"

double OTC_DESK_PORT = parseInt(OR((process->env->OTC_DESK_PORT), (std::string("4444"))));
string BASE_URL = std::string("http://localhost:") + OTC_DESK_PORT + string_empty;

void Main(void)
{
}

MAIN
