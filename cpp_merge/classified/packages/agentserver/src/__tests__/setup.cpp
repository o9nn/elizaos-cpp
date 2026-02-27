#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/setup.h"

object originalConsole = object{
    object::pair{std:("log"), console->log}, 
    object::pair{std:("warn"), console->warn}, 
    object::pair{std:("error"), console->error}, 
    object::pair{std:("info"), console->info}
};

void Main(void)
{
    process->env->NODE_ENV = std:("test");
    process->env->LOG_LEVEL = std:("error");
    beforeAll([=]() mutable
    {
        console->log = [=]() mutable
        {
        };
        console->warn = [=]() mutable
        {
        };
        console->error = [=]() mutable
        {
        };
        console->info = [=]() mutable
        {
        };
        logger->level = std:("error");
    }
    );
    afterAll([=]() mutable
    {
        console->log = originalConsole["log"];
        console->warn = originalConsole["warn"];
        console->error = originalConsole["error"];
        console->info = originalConsole["info"];
    }
    );
    beforeEach([=]() mutable
    {
        mock->restore();
    }
    );
    afterEach([=]() mutable
    {
        mock->restore();
    }
    );
}

MAIN
