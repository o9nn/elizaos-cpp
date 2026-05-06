#include "logger.hpp"
#include <string>

object logger = object{
    object::pair{std::string("error"), [=](auto message, auto error = std::nullopt) mutable
    {
        console->error(message, error);
    }
    }, 
    object::pair{std::string("info"), [=](auto message, auto data = std::nullopt) mutable
    {
        console->log(message, data);
    }
    }, 
    object::pair{std::string("warn"), [=](auto message, auto data = std::nullopt) mutable
    {
        console->warn(message, data);
    }
    }
};

void Main(void)
{
}

MAIN
