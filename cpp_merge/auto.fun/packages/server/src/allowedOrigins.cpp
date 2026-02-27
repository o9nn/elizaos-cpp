#include "allowedOrigins.hpp"

array<string> envAllowedOrigins = (process->env->ALLOWED_ORIGINS) ? any(process->env->ALLOWED_ORIGINS->split(std:(","))->map([=](auto origin) mutable
{
    return origin->trim();
}
)) (nullptr);
array<string> allowedOrigins = OR((envAllowedOrigins), (array<string>{ std:("http://localhost:3000") }));

void Main(void)
{
    console->log(std:("envAllowedOrigins"), envAllowedOrigins);
}

MAIN
