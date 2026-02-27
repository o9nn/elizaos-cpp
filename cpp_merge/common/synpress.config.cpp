#include "synpress.config.h"

double OTC_DESK_PORT = parseInt(OR((process->env->OTC_DESK_PORT), (std:("4444"))));
string BASE_URL = std:("http://localhost:") + OTC_DESK_PORT + string_empty;

void Main(void)
{
}

MAIN
