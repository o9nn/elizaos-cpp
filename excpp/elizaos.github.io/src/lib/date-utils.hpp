#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Extracts date from a title string in format "elizaos Eliza (2025-01-12)"
 * First tries to extract from parentheses, then falls back to direct date pattern matching
 */
std::string extractDateFromTitle(const std::string& title);

/**
 * Validates if a string is in YYYY-MM-DD format
 */
bool isValidDateString(const std::string& dateStr);

/**
 * Normalizes date string by replacing hyphens with underscores
 * e.g., "2025-01-12" -> "2025_01_12"
 */
std::string normalizeDate(const std::string& date);

/**
 * Denormalizes date string by replacing underscores with hyphens
 * e.g., "2025_01_12" -> "2025-01-12"
 */
std::string denormalizeDate(const std::string& date);

/**
 * Extracts date from any filename containing a date pattern YYYY-MM-DD or YYYY_MM_DD
 */
std::string extractDateFromFilename(const std::string& filename);

/**
 * Converts a date to YYYY-MM-DD format string
 * @param date - Date object, timestamp, or date string that can be parsed by new Date()
 * @returns string in YYYY-MM-DD format
 */
std::string toDateString(const std::variant<std::string, double, Date>& date);

/**
 * Converts a date to UTC midnight (00:00:00.000Z)
 * @param date - Date object, timestamp, or date string that can be parsed by new Date()
 * @returns Date object set to UTC midnight
 */
UTCDate toUTCMidnight(const std::variant<std::string, double, Date>& date);
using IntervalType = std::variant<"day", "week", "month">;

struct TimeInterval {
    UTCDate intervalStart;
    UTCDate intervalEnd;
    IntervalType intervalType;
};

/**
 * Generates a formatted name for a time interval
 * @param interval - The time interval object
 * @returns Formatted name for the interval
 */
std::string generateIntervalName(TimeInterval interval);
/**
 * Get time period text based on interval type
 */

struct DateRangeOptions {
    std::optional<std::string> after;
    std::optional<std::string> before;
    std::optional<std::variant<std::string, double>> days;
    std::optional<std::string> defaultStartDate;
};

struct DateRange {
    std::string startDate;
    std::string endDate;
};

/**
 * Calculates a date range based on command line options
 * @param options - Object containing after, before, and days options
 * @returns Object with startDate (optional) and endDate as strings in YYYY-MM-DD format
 */
DateRange calculateDateRange(DateRangeOptions options);

/**
 * Generates a date range from a start date to end date (today by default)
 * @param startDate - The start date for the range
 * @param endDate - The end date for the range (defaults to current date)
 * @param descending - Whether to sort dates in descending order (newest first)
 * @returns Array of date strings in YYYY-MM-DD format
 */

  // Create date objects for each day between start and end
    // Format date as YYYY-MM-DD

    // Move to next day

  // Sort dates if needed

void formatReadableDate(const std::variant<std::string, Date>& date);

/**
 * Formats a date into a human-readable timeframe title based on interval type
 * @param date - The date to format
 * @param intervalType - The type of interval (day, week, month)
 * @returns Formatted timeframe title
 */
std::string formatTimeframeTitle(const std::variant<UTCDate, Date, std::string>& date, IntervalType intervalType, std::optional<std::any> formatOptions);

/**
 * Converts interval type to a title form
 * @param intervalType - The type of interval (day, week, month)
 * @returns Title form of the interval type
 */
std::string getIntervalTypeTitle(IntervalType intervalType);

/**
 * Calculates the precise start and end boundaries for a given interval type and date.
 * This ensures consistent interval definitions across the application.
 *
 * @param date - The reference date for the interval.
 * @param intervalType - The type of interval (day, week, month).
 * @returns An object containing the intervalStart and intervalEnd as UTCDate objects.
 */

      // Ensure we start from the Sunday of the week containing the date

      // Ensure we start from the first day of the month

      // Exhaustive check

/**
 * Generates time intervals for a given date range and interval type
 * Can be used for both pipeline steps and generating static params for routes
 */

  // Convert dates to UTC midnight and adjust based on interval type

      // Get start of week (Sunday) for the start date

      // Get first day of next week after the end date

      // Start from first day of the start month

      // End at the first day of the month after the end date

  // Handle single-day case

    // Use the helper function to determine the precise boundaries for the current interval start

    // Move to next interval start using the calculated end

/**
 * Formats interval date for URL path segments
 * @param interval - The time interval
 * @returns Formatted date string or array based on interval type
 */
std::vector<std::string> formatIntervalForPath(TimeInterval interval); /**
 * Find adjacent intervals using the same logic as generateTimeIntervalsForDateRange
 */

  // Generate a range of intervals spanning before and after the current date
  // We'll use a wider date range around the current interval

  // Find the current interval's index

    // Should never happen but handle the case anyway

  // Determine previous interval (if any)

  // Determine next interval (if any)

  // Format interval dates for the URL

IntervalType getIntervalTypeFromDateRange(const std::any& dateRange);

} // namespace elizaos
