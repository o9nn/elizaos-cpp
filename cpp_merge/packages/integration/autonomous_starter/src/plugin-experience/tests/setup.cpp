#include "setup.hpp"
#include <string>

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
    global->console = utils::assign(object{
        , 
        object::pair{std::string("log"), vi->fn()}, 
        object::pair{std::string("debug"), vi->fn()}, 
        object::pair{std::string("info"), vi->fn()}, 
        object::pair{std::string("warn"), vi->fn()}, 
        object::pair{std::string("error"), vi->fn()}
    }, console);
    process->env->NODE_ENV = std::string("test");
}

MAIN
