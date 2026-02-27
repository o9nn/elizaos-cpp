#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/monitoring/logger.h"

object logger = object{
    object::pair{std:("error"), [=](auto message, auto error = undefined) mutable
    {
        console->error(message, error);
    }
    }, 
    object::pair{std:("info"), [=](auto message, auto data = undefined) mutable
    {
        console->log(message, data);
    }
    }, 
    object::pair{std:("warn"), [=](auto message, auto data = undefined) mutable
    {
        console->warn(message, data);
    }
    }
};

void Main(void)
{
}

MAIN
