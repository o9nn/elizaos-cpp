#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/utils/dateTime.h"

boolean isAvailableNow(array<string> workDays, object workHours, string timeZone)
{
    try
    {
        auto now = std::make_shared<Date>();
        auto options = object{
            object::pair{std::string("timeZone"), std::string("timeZone")}, 
            object::pair{std::string("weekday"), std::string("long")}
        };
        auto dayName = const_(((std::make_shared<Intl::DateTimeFormat>(std::string("en-US"), options)))->format(now)->split(std::string(",")))[0];
        if (!workDays->includes(dayName)) {
            return false;
        }
        auto timeOptions = object{
            object::pair{std::string("timeZone"), std::string("timeZone")}, 
            object::pair{std::string("hour"), std::string("numeric")}, 
            object::pair{std::string("minute"), std::string("numeric")}, 
            object::pair{std::string("hour12"), false}
        };
        auto currentTime = ((std::make_shared<Intl::DateTimeFormat>(std::string("en-US"), timeOptions)))->format(now);
        auto [currentHour, currentMinute] = currentTime->split(std::string(":"))->map(Number);
        auto [startHour, startMinute] = workHours["start"]->split(std::string(":"))->map(Number);
        auto [endHour, endMinute] = workHours["end"]->split(std::string(":"))->map(Number);
        auto currentTimeMinutes = currentHour * 60 + currentMinute;
        auto startTimeMinutes = startHour * 60 + startMinute;
        auto endTimeMinutes = endHour * 60 + endMinute;
        return AND((currentTimeMinutes >= startTimeMinutes), (currentTimeMinutes <= endTimeMinutes));
    }
    catch (const any& error)
    {
        console->error(std::string("Error checking availability:"), error);
        return false;
    }
};


std::shared_ptr<Date> calculateNextCheckIn(array<string> workDays, object workHours, string timeZone, double frequencyHours)
{
    auto now = std::make_shared<Date>();
    auto getDateWithTime = [=](auto date, auto timeStr) mutable
    {
        auto [hours, minutes] = timeStr->split(std::string(":"))->map(Number);
        auto result = std::make_shared<Date>(date);
        result->setHours(hours, minutes, 0, 0);
        return result;
    };
    auto nextDate = std::make_shared<Date>(now);
    auto daysChecked = 0;
    while (daysChecked < 14)
    {
        auto options = object{
            object::pair{std::string("timeZone"), std::string("timeZone")}, 
            object::pair{std::string("weekday"), std::string("long")}
        };
        auto dayName = ((std::make_shared<Intl::DateTimeFormat>(std::string("en-US"), options)))->format(nextDate);
        if (workDays->includes(dayName)) {
            auto startTime = getDateWithTime(nextDate, workHours["start"]);
            if (nextDate < startTime) {
                return startTime;
            }
            auto endTime = getDateWithTime(nextDate, workHours["end"]);
            if (nextDate <= endTime) {
                auto nextCheckIn = std::make_shared<Date>(nextDate->getTime() + frequencyHours * 60 * 60 * 1000);
                if (nextCheckIn <= endTime) {
                    return nextCheckIn;
                }
            }
        }
        nextDate->setDate(nextDate->getDate() + 1);
        nextDate = getDateWithTime(nextDate, workHours["start"]);
        daysChecked++;
    }
    auto tomorrow = std::make_shared<Date>(now);
    tomorrow->setDate(tomorrow->getDate() + 1);
    tomorrow->setHours(9, 0, 0, 0);
    return tomorrow;
};


string formatDate(std::shared_ptr<Date> date, string timeZone)
{
    auto options = object{
        object::pair{std::string("weekday"), std::string("long")}, 
        object::pair{std::string("year"), std::string("numeric")}, 
        object::pair{std::string("month"), std::string("long")}, 
        object::pair{std::string("day"), std::string("numeric")}, 
        object::pair{std::string("hour"), std::string("2-digit")}, 
        object::pair{std::string("minute"), std::string("2-digit")}, 
        object::pair{std::string("timeZone"), std::string("timeZone")}
    };
    return ((std::make_shared<Intl::DateTimeFormat>(std::string("en-US"), options)))->format(date);
};


