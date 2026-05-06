#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/providers/time.h"

std::shared_ptr<Provider> timeProvider = object{
    object::pair{std::string("name"), std::string("TIME")}, 
    object::pair{std::string("get"), [=](auto _runtime, auto _message) mutable
    {
        auto currentDate = std::make_shared<Date>();
        auto options = object{
            object::pair{std::string("timeZone"), std::string("UTC")}, 
            object::pair{std::string("dateStyle"), as<std::shared_ptr<const>>(std::string("full"))}, 
            object::pair{std::string("timeStyle"), as<std::shared_ptr<const>>(std::string("long"))}
        };
        auto humanReadable = ((std::make_shared<Intl::DateTimeFormat>(std::string("en-US"), options)))->format(currentDate);
        return object{
            object::pair{std::string("data"), object{
                object::pair{std::string("time"), currentDate}
            }}, 
            object::pair{std::string("values"), object{
                object::pair{std::string("time"), humanReadable}
            }}, 
            object::pair{std::string("text"), std::string("The current date and time is ") + humanReadable + std::string(". Please use this as your reference for any time-based operations or responses.")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
