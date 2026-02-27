#include "setup.hpp"
#include <string>

object originalConsole = object{
    object::pair{std::string("log"), console->log}, 
    object::pair{std::string("warn"), console->warn}, 
    object::pair{std::string("error"), console->error}, 
    object::pair{std::string("info"), console->info}
};

void Main(void)
{
    process->env->NODE_ENV = std::string("test");
    process->env->LOG_LEVEL = std::string("error");
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
        logger->level = std::string("error");
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
