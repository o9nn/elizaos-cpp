#include "elizas-list/src/lib/redis.h"

std::any redis = std::make_shared<Redis>(OR((process->env->REDIS_URL), (std::string("redis://localhost:6379"))));

void Main(void)
{
}

MAIN
