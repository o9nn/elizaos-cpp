#include "setup.hpp"
#include <string>

std::function<array<any>()> createMockHistory = [=]() mutable
{
    return array<object>{ object{
        object::pair{std::string("role"), std::string("system")}, 
        object::pair{std::string("content"), std::string("You are a helpful assistant.")}, 
        object::pair{std::string("messageType"), as<std::shared_ptr<const>>(std::string("thought"))}
    }, object{
        object::pair{std::string("role"), std::string("user")}, 
        object::pair{std::string("content"), std::string("Hello")}, 
        object::pair{std::string("messageType"), as<std::shared_ptr<const>>(std::string("action"))}
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
        object::pair{std::string("path"), path->join(__dirname, std::string(".."), std::string(".env.test"))}
    });
    process->env->NODE_ENV = std::string("test");
    process->env->LOG_LEVEL = OR((process->env->LOG_LEVEL), (std::string("error")));
    if (process->env->QUIET_TESTS == std::string("true")) {
        global->console = utils::assign(object{
            , 
            object::pair{std::string("log"), jest->fn()}, 
            object::pair{std::string("debug"), jest->fn()}, 
            object::pair{std::string("info"), jest->fn()}, 
            object::pair{std::string("warn"), jest->fn()}, 
            object::pair{std::string("error"), jest->fn()}
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
