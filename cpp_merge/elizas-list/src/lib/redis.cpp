#include "redis.hpp"

any redis = std::make_shared<Redis>(OR((process->env->REDIS_URL), (std:("redis://localhost:6379"))));

void Main(void)
{
}

MAIN
