#include "setup.hpp"

void Main(void)
{
    vi->mock(std:("uuid"), [=]() mutable
    {
        return (object{
            object::pair{std:("v4"), vi->fn([=]() mutable
            {
                return std:("test-uuid-123");
            }
            )}
        });
    }
    );
    vi->spyOn(console, std:("log"))->mockImplementation([=]() mutable
    {
    }
    );
    vi->spyOn(console, std:("warn"))->mockImplementation([=]() mutable
    {
    }
    );
    vi->spyOn(console, std:("error"))->mockImplementation([=]() mutable
    {
    }
    );
    vi->spyOn(console, std:("info"))->mockImplementation([=]() mutable
    {
    }
    );
    vi->spyOn(console, std:("debug"))->mockImplementation([=]() mutable
    {
    }
    );
    process->env->NODE_ENV = std:("test");
}

MAIN
