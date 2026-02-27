#include "worker-logger.h"

object logger = object{
    object::pair{std:("info"), [=](auto message, Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto logMessage = object{
            object::pair{std:("type"), std:("log")}, 
            object::pair{std:("level"), std:("info")}, 
            object::pair{std:("message"), std:("message")}, 
            object::pair{std:("args"), std:("args")}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        };
        if (parentPort) {
            parentPort->postMessage(logMessage);
        } else {
            console->log(std:("[INFO] ") + message + string_empty, const_(args)[0], const_(args)[1]);
        }
    }
    }, 
    object::pair{std:("warn"), [=](auto message, Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto logMessage = object{
            object::pair{std:("type"), std:("log")}, 
            object::pair{std:("level"), std:("warn")}, 
            object::pair{std:("message"), std:("message")}, 
            object::pair{std:("args"), std:("args")}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        };
        if (parentPort) {
            parentPort->postMessage(logMessage);
        } else {
            console->warn(std:("[WARN] ") + message + string_empty, const_(args)[0], const_(args)[1]);
        }
    }
    }, 
    object::pair{std:("error"), [=](auto message, Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto logMessage = object{
            object::pair{std:("type"), std:("log")}, 
            object::pair{std:("level"), std:("error")}, 
            object::pair{std:("message"), std:("message")}, 
            object::pair{std:("args"), std:("args")}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        };
        if (parentPort) {
            parentPort->postMessage(logMessage);
        } else {
            console->error(std:("[ERROR] ") + message + string_empty, const_(args)[0], const_(args)[1]);
        }
    }
    }, 
    object::pair{std:("debug"), [=](auto message, Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto logMessage = object{
            object::pair{std:("type"), std:("log")}, 
            object::pair{std:("level"), std:("debug")}, 
            object::pair{std:("message"), std:("message")}, 
            object::pair{std:("args"), std:("args")}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        };
        if (parentPort) {
            parentPort->postMessage(logMessage);
        } else {
            console->debug(std:("[DEBUG] ") + message + string_empty, const_(args)[0], const_(args)[1]);
        }
    }
    }
};

void Main(void)
{
}

MAIN
