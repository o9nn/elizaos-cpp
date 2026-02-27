#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/workers/worker-logger.h"

object logger = object{
    object::pair{std::string("info"), [=](auto message, Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto logMessage = object{
            object::pair{std::string("type"), std::string("log")}, 
            object::pair{std::string("level"), std::string("info")}, 
            object::pair{std::string("message"), std::string("message")}, 
            object::pair{std::string("args"), std::string("args")}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        };
        if (parentPort) {
            parentPort->postMessage(logMessage);
        } else {
            console->log(std::string("[INFO] ") + message + string_empty, const_(args)[0], const_(args)[1]);
        }
    }
    }, 
    object::pair{std::string("warn"), [=](auto message, Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto logMessage = object{
            object::pair{std::string("type"), std::string("log")}, 
            object::pair{std::string("level"), std::string("warn")}, 
            object::pair{std::string("message"), std::string("message")}, 
            object::pair{std::string("args"), std::string("args")}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        };
        if (parentPort) {
            parentPort->postMessage(logMessage);
        } else {
            console->warn(std::string("[WARN] ") + message + string_empty, const_(args)[0], const_(args)[1]);
        }
    }
    }, 
    object::pair{std::string("error"), [=](auto message, Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto logMessage = object{
            object::pair{std::string("type"), std::string("log")}, 
            object::pair{std::string("level"), std::string("error")}, 
            object::pair{std::string("message"), std::string("message")}, 
            object::pair{std::string("args"), std::string("args")}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        };
        if (parentPort) {
            parentPort->postMessage(logMessage);
        } else {
            console->error(std::string("[ERROR] ") + message + string_empty, const_(args)[0], const_(args)[1]);
        }
    }
    }, 
    object::pair{std::string("debug"), [=](auto message, Args... args_) mutable
    {
        array<any> args = array<any>{args_...};
        auto logMessage = object{
            object::pair{std::string("type"), std::string("log")}, 
            object::pair{std::string("level"), std::string("debug")}, 
            object::pair{std::string("message"), std::string("message")}, 
            object::pair{std::string("args"), std::string("args")}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        };
        if (parentPort) {
            parentPort->postMessage(logMessage);
        } else {
            console->debug(std::string("[DEBUG] ") + message + string_empty, const_(args)[0], const_(args)[1]);
        }
    }
    }
};

void Main(void)
{
}

MAIN
