#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/setup.h"

std::function<array<any>()> createMockHistory = [=]() mutable
{
    return array<object>{ object{
        object::pair{std:("role"), std:("system")}, 
        object::pair{std:("content"), std:("You are a helpful assistant.")}, 
        object::pair{std:("messageType"), as<std::shared_ptr<const>>(std:("thought"))}
    }, object{
        object::pair{std:("role"), std:("user")}, 
        object::pair{std:("content"), std:("Hello")}, 
        object::pair{std:("messageType"), as<std::shared_ptr<const>>(std:("action"))}
    } };
};
std::function<std::shared_ptr<Promise<any>>(double)> delay = [=](auto ms) mutable
{
    return std::make_shared<Promise>([=](auto resolve) mutable
    {
        return setTimeout(resolve, ms);
    }
    );
};

void Main(void)
{
    dotenv->config(object{
        object::pair{std:("path"), path->join(__dirname, std:(".."), std:(".env.test"))}
    });
    process->env->NODE_ENV = std:("test");
    process->env->LOG_LEVEL = OR((process->env->LOG_LEVEL), (std:("error")));
    if (process->env->QUIET_TESTS == std:("true")) {
        global->console = utils::assign(object{
            , 
            object::pair{std:("log"), jest->fn()}, 
            object::pair{std:("debug"), jest->fn()}, 
            object::pair{std:("info"), jest->fn()}, 
            object::pair{std:("warn"), jest->fn()}, 
            object::pair{std:("error"), jest->fn()}
        }, console);
    }
    expect->extend(object{
    });
    afterAll([=]() mutable
    {
        std::async([=]() { delay(100); });
    }
    );
}

MAIN
