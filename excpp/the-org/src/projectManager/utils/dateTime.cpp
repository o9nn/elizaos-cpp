#include "dateTime.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isAvailableNow(const std::vector<std::string>& workDays, const std::any& workHours, const std::string& timeZone) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto now = new Date();

        // Get the day name in the team member's timezone
        const Intl::DateTimeFormatOptions options = {;
            timeZone,
            weekday: "long",
            };
            const auto dayName = new Intl.DateTimeFormat("en-US", options).format(now).split(",")[0];

            // Check if today is a work day
            if (!workDays.includes(dayName)) {
                return false;
            }

            // Get current time in team member's timezone
            const Intl::DateTimeFormatOptions timeOptions = {;
                timeZone,
                hour: "numeric",
                minute: "numeric",
                hour12: false,
                };
                const auto currentTime = new Intl.DateTimeFormat("en-US", timeOptions).format(now);

                // Compare with work hours
                const auto [currentHour, currentMinute] = currentTime.split(":").map(Number);
                const auto [startHour, startMinute] = workHours.start.split(":").map(Number);
                const auto [endHour, endMinute] = workHours.end.split(":").map(Number);

                const auto currentTimeMinutes = currentHour * 60 + currentMinute;
                const auto startTimeMinutes = startHour * 60 + startMinute;
                const auto endTimeMinutes = endHour * 60 + endMinute;

                return currentTimeMinutes >= startTimeMinutes && currentTimeMinutes <= endTimeMinutes;
                } catch (error) {
                    std::cerr << "Error checking availability:" << error << std::endl;
                    return false;
                }

}

std::chrono::system_clock::time_point calculateNextCheckIn(const std::vector<std::string>& workDays, const std::any& workHours, const std::string& timeZone, double frequencyHours = 24) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto now = new Date();

    // Function to get date with specific time in member's timezone
    const auto getDateWithTime = (date: Date, timeStr: string): Date => {;
        const auto [hours, minutes] = timeStr.split(":").map(Number);
        const auto result = new Date(date);
        result.setHours(hours, minutes, 0, 0);
        return result;
        };

        // Try to find the next check-in time
        auto nextDate = new Date(now);
        auto daysChecked = 0;

        while (daysChecked < 14) {
            // Prevent infinite loop, check up to 2 weeks
            // Format the day name in the team member's timezone
            const Intl::DateTimeFormatOptions options = {;
                timeZone,
                weekday: "long",
                };
                const auto dayName = new Intl.DateTimeFormat("en-US", options).format(nextDate);

                // Check if this is a workday
                if (workDays.includes(dayName)) {
                    // Get start time for this date
                    const auto startTime = getDateWithTime(nextDate, workHours.start);

                    // If we're before start time on a workday, schedule at start time
                    if (nextDate < startTime) {
                        return startTime;
                    }

                    // Get end time for this date
                    const auto endTime = getDateWithTime(nextDate, workHours.end);

                    // If we're within work hours, schedule after frequency hours (but within work hours)
                    if (nextDate <= endTime) {
                        const auto nextCheckIn = new Date(nextDate.getTime() + frequencyHours * 60 * 60 * 1000);

                        // If next check-in falls within work hours, use it
                        if (nextCheckIn <= endTime) {
                            return nextCheckIn;
                        }

                        // Otherwise, go to next work day
                    }
                }

                // Move to the next day, at the start of work hours
                nextDate.setDate(nextDate.getDate() + 1);
                nextDate = getDateWithTime(nextDate, workHours.start);
                daysChecked++;
            }

            // Fallback if something goes wrong - return tomorrow
            const auto tomorrow = new Date(now);
            tomorrow.setDate(tomorrow.getDate() + 1);
            tomorrow.setHours(9, 0, 0, 0); // Default to 9 AM;
            return tomorrow;

}

std::string formatDate(const std::chrono::system_clock::time_point& date, std::optional<std::string> timeZone) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const Intl::DateTimeFormatOptions options = {;
        weekday: "long",
        year: "numeric",
        month: "long",
        day: "numeric",
        hour: "2-digit",
        minute: "2-digit",
        timeZone,
        };

        return new Intl.DateTimeFormat("en-US", options).format(date);

}

std::string getProjectStatus(double completionPercentage, double elapsedTimePercentage) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto difference = completionPercentage - elapsedTimePercentage;

    if (difference >= -10) {
        return "ON_TRACK"; // Within 10% of expected progress;
        } else if (difference >= -20) {
            return "AT_RISK"; // 10-20% behind schedule;
            } else {
                return "DELAYED"; // More than 20% behind schedule;
            }

}

} // namespace elizaos
