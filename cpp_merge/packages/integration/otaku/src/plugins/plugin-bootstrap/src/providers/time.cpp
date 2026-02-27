#include "time.hpp"

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
                object::pair{std:("time"), std:("🚨 CRITICAL: The current date and time is ") + humanReadable + std:(". You MUST use this exact time as your absolute reference for ALL time-based operations, calculations, and responses. DO NOT use any other time reference. This is the ONLY source of truth for current time.")}
            }}, 
            object::pair{std:("text"), std:("🚨 CRITICAL: The current date and time is ") + humanReadable + std:(". You MUST use this exact time as your absolute reference for ALL time-based operations, calculations, and responses. DO NOT use any other time reference. This is the ONLY source of truth for current time.")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
