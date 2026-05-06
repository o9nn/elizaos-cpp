#include "logger.hpp"
#include <string>

Record<any, LogMethod> logger = object{
    object::pair{std::string("trace"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->trace->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("debug"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->debug->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("success"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->debug->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("progress"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->debug->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("log"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->info->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("info"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->info->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("warn"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->warn->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("error"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->error->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("fatal"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->fatal->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std::string("clear"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->clear->apply(coreLogger, array<string>{ String(message), rest });
    }
    }
};
std::shared_ptr<Record<any, std::shared_ptr<LogMethod>>> elizaLogger = logger;

void Main(void)
{
}

MAIN
