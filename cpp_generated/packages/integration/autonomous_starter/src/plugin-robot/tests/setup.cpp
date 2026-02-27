#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/tests/setup.h"

void Main(void)
{
    vi->mock(std::string("uuid"), [=]() mutable
    {
        return (object{
            object::pair{std::string("v4"), vi->fn([=]() mutable
            {
                return std::string("test-uuid-123");
            }
            )}
        });
    }
    );
    vi->spyOn(console, std::string("log"))->mockImplementation([=]() mutable
    {
    }
    );
    vi->spyOn(console, std::string("warn"))->mockImplementation([=]() mutable
    {
    }
    );
    vi->spyOn(console, std::string("error"))->mockImplementation([=]() mutable
    {
    }
    );
    vi->spyOn(console, std::string("info"))->mockImplementation([=]() mutable
    {
    }
    );
    vi->spyOn(console, std::string("debug"))->mockImplementation([=]() mutable
    {
    }
    );
    process->env->NODE_ENV = std::string("test");
}

MAIN
