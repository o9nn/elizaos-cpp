#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/logger.h"

std::function<string()> getTimestamp = [=]() mutable
{
    return ((std::make_shared<Date>()))->toISOString();
};
object logger = object{
    object::pair{std:("log"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        console->log(std:("[") + getTimestamp() + std:("]"), const_(args)[0], const_(args)[1]);
    }
    }, 
    object::pair{std:("info"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        console->info(std:("[") + getTimestamp() + std:("]"), const_(args)[0], const_(args)[1]);
    }
    }, 
    object::pair{std:("warn"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        console->warn(std:("[") + getTimestamp() + std:("]"), const_(args)[0], const_(args)[1]);
    }
    }, 
    object::pair{std:("error"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        console->error(std:("[") + getTimestamp() + std:("]"), const_(args)[0], const_(args)[1]);
    }
    }
};

void Main(void)
{
}

MAIN
