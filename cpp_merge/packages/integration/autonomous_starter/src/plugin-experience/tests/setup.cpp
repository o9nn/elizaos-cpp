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
    global->console = utils::assign(object{
        , 
        object::pair{std:("log"), vi->fn()}, 
        object::pair{std:("debug"), vi->fn()}, 
        object::pair{std:("info"), vi->fn()}, 
        object::pair{std:("warn"), vi->fn()}, 
        object::pair{std:("error"), vi->fn()}
    }, console);
    process->env->NODE_ENV = std:("test");
}

MAIN
