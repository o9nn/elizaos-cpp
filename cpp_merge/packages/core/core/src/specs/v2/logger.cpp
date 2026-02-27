#include "logger.hpp"

Record<any, LogMethod> logger = object{
    object::pair{std:("trace"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->trace->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("debug"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->debug->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("success"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->debug->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("progress"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->debug->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("log"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->info->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("info"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->info->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("warn"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->warn->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("error"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->error->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("fatal"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto [message, rest] = args;
        return coreLogger->fatal->apply(coreLogger, array<string>{ String(message), rest });
    }
    }, 
    object::pair{std:("clear"), [=](Args... args_) mutable
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
