#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/providers/time.h"

std::shared_ptr<Provider> timeProvider = object{
    object::pair{std:("name"), std:("TIME")}, 
    object::pair{std:("get"), [=](auto _runtime, auto _message) mutable
    {
        auto currentDate = std::make_shared<Date>();
        auto options = object{
            object::pair{std:("timeZone"), std:("UTC")}, 
            object::pair{std:("dateStyle"), as<std::shared_ptr<const>>(std:("full"))}, 
            object::pair{std:("timeStyle"), as<std::shared_ptr<const>>(std:("long"))}
        };
        auto humanReadable = ((std::make_shared<Intl::DateTimeFormat>(std:("en-US"), options)))->format(currentDate);
        return object{
            object::pair{std:("data"), object{
                object::pair{std:("time"), currentDate}
            }}, 
            object::pair{std:("values"), object{
                object::pair{std:("time"), humanReadable}
            }}, 
            object::pair{std:("text"), std:("The current date and time is ") + humanReadable + std:(". Please use this as your reference for any time-based operations or responses.")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
