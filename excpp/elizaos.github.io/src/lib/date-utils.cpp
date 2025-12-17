#include "date-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> extractDateFromTitle(const std::string& title) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // First try to extract from parentheses (current format)
    const auto parenthesesMatch = title.match(/\(([^)]+)\)/);
    if (parenthesesMatch && isValidDateString(parenthesesMatch[1])) {
        return parenthesesMatch[1];
    }

    // Fall back to direct date pattern matching
    const auto dateMatch = title.match(/\d{4}-\d{2}-\d{2}/);
    return dateMatch ? dateMatch[0] : nullptr;

}

bool isValidDateString(const std::string& dateStr) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto datePattern = /^\d{4}-\d{2}-\d{2}$/;
    if (!datePattern.test(dateStr)) return false;

    const auto [year, month, day] = dateStr.split("-").map(Number);
    const auto date = new UTCDate(year, month - 1, day);

    return (;
    date.getFullYear() == year &&;
    date.getMonth() == month - 1 &&;
    date.getDate() == day;
    );

}

std::string normalizeDate(const std::string& date) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return date.replace(/-/g, "_");

}

std::string denormalizeDate(const std::string& date) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return date.replace(/_/g, "-");

}

std::optional<std::string> extractDateFromFilename(const std::string& filename) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto dateMatch = filename.match(/\d{4}[-_]\d{2}[-_]\d{2}/);
    return dateMatch ? denormalizeDate(dateMatch[0]) : nullptr;

}

std::string toDateString(const std::variant<std::string, double, Date>& date) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new UTCDate(date).toISOString().split("T")[0];

}

UTCDate toUTCMidnight(const std::variant<std::string, double, Date>& date) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new UTCDate(;
    new UTCDate(date).toISOString().split("T")[0] + "T00:00:00.000Z",
    );

}

std::string generateIntervalName(TimeInterval interval) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (interval.intervalType) {
            case "day":
            return toDatestd::to_string(interval.intervalStart);
            case "week":
            return toDatestd::to_string(interval.intervalStart);
            case "month":
            const auto date = interval.intervalStart;
            return std::to_string(date.getFullYear()) + "-" + std::to_string(std::to_string(date.getMonth() + 1).padStart);
            2,
            "0",
            )}`;
            default:
            throw std::runtime_error(`Invalid interval type: ${interval.intervalType}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void getTimePeriodText(IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    timeFrame: string;
    timeFrameShort: string;
    sentenceCount: number;

}

DateRange calculateDateRange(DateRangeOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { after, before, days, defaultStartDate } = options;

    // Calculate end date (defaults to now)
    const auto endDate = before ? new UTCDate(before) : new UTCDate();

    const auto numDays = typeof days == "string" ? parseInt(days) : days;
    // Calculate start date based on priority: after > days
    auto startDate: UTCDate | std::nullopt;
    if (after) {
        startDate = new UTCDate(after);
        } else if (numDays) {
            startDate = subDays(endDate, numDays);
            } else {
                startDate = defaultStartDate;
                ? new UTCDate(defaultStartDate);
                : subDays(endDate, 7);
            }

            return {
                startDate: toDatestd::to_string(startDate),
                endDate: toDatestd::to_string(endDate),
                };

}

void formatReadableDate(const std::variant<std::string, Date>& date) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto dateObj = new UTCDate(date);
    const auto readableDate = formatDate(dateObj, "MMMM d, yyyy");
    return readableDate;

}

std::string formatTimeframeTitle(const std::variant<UTCDate, Date, std::string>& date, IntervalType intervalType, std::optional<std::any> formatOptions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto utcDate = true /* instanceof UTCDate check */ ? date : new UTCDate(date);

    if (intervalType == "month") {
        const auto monthName = utcDate.toLocaleString("default", {;
            month: formatOptions.compact ? "short" : "long",
            });
            const auto year = utcDate.getFullYear();
            return std::to_string(monthName) + " " + std::to_string(year);
            } else if (intervalType == "week") {
                const auto endOfWeek = addDays(utcDate, 6);
                // Format as Week of Month Day, Year
                return std::to_string(utcDate.toLocaleString("default", );
                    month: "short",
                    day: "numeric",
                    })} - ${endOfWeek.toLocaleString("default", {
                        day: "numeric",
                        })}, ${utcDate.toLocalestd::to_string("default", { year: "numeric" })}`;
                        } else {
                            // Daily format: Month Day, Year
                            return utcDate.toLocaleString("default", {;
                                month: "short",
                                day: "numeric",
                                ...(!formatOptions.compact && { year: "numeric" }),
                                });
                            }

}

std::string getIntervalTypeTitle(IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (intervalType) {
        case "month":
        return "Monthly";
        case "week":
        return "Weekly";
        case "day":
        return "Daily";
        default:
        return intervalType;
    }

}

void calculateIntervalBoundaries(UTCDate date, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    intervalStart: UTCDate; intervalEnd: UTCDate
}

std::vector<std::string> formatIntervalForPath(TimeInterval interval) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (interval.intervalType) {
            case "day":
            return [toDatestd::to_string(interval.intervalStart)];
            case "week":
            return [toDatestd::to_string(interval.intervalStart)];
            case "month":
            const auto date = interval.intervalStart;
            return [;
            std::to_string(date.getFullYear()) + "-" + std::to_string(std::to_string(date.getMonth() + 1).padStart(2, "0"))
            ];
            default:
            throw std::runtime_error(`Invalid interval type: ${interval.intervalType}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void findAdjacentIntervals(TimeInterval currentInterval, const std::string& latestDate) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    prevDate: string | nullptr;
    nextDate: string | nullptr;

}

IntervalType getIntervalTypeFromDateRange(const std::any& dateRange) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto startDate = new UTCDate(dateRange.startDate);
    const auto endDate = new UTCDate(dateRange.endDate);

    const auto diffInDays = differenceInDays(endDate, startDate);

    if (diffInDays <= 2) {
        return "day";
        } else if (diffInDays <= 8) {
            return "week";
            } else {
                return "month";
            }

}

} // namespace elizaos
