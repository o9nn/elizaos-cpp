#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/allowedOrigins.h"

array<string> envAllowedOrigins = (process->env->ALLOWED_ORIGINS) ? any(process->env->ALLOWED_ORIGINS->split(std::string(","))->map([=](auto origin) mutable
{
    return origin->trim();
}
)) : any(nullptr);
array<string> allowedOrigins = OR((envAllowedOrigins), (array<string>{ std::string("http://localhost:3000") }));

void Main(void)
{
    console->log(std::string("envAllowedOrigins"), envAllowedOrigins);
}

MAIN
