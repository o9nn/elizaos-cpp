#include "logger.hpp"
#include <string>

std::function<string()> getTimestamp = [=]() mutable
{
    return ((std::make_shared<Date>()))->toISOString();
};
object logger = object{
    object::pair{std::string("log"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        console->log(std::string("[") + getTimestamp() + std::string("]"), const_(args)[0], const_(args)[1]);
    }
    }, 
    object::pair{std::string("info"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        console->info(std::string("[") + getTimestamp() + std::string("]"), const_(args)[0], const_(args)[1]);
    }
    }, 
    object::pair{std::string("warn"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        console->warn(std::string("[") + getTimestamp() + std::string("]"), const_(args)[0], const_(args)[1]);
    }
    }, 
    object::pair{std::string("error"), [=](Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        console->error(std::string("[") + getTimestamp() + std::string("]"), const_(args)[0], const_(args)[1]);
    }
    }
};

void Main(void)
{
}

MAIN
