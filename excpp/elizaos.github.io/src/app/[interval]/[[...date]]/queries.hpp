#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<void> getLatestAvailableDate();

/**
 * Parse date string based on interval type format
 * @param dateStr - Date string to parse
 * @param intervalType - Interval type (day, week, month)
 * @returns TimeInterval object or null if invalid
 */
std::optional<TimeInterval> parseIntervalDate(const std::string& dateStr, IntervalType intervalType);

/**
 * Get metrics for repositories for a specific interval
 * @param date - Date string in format based on interval type
 * @param intervalType - Type of interval (day, week, month)
 * @returns Object with metrics for the specified interval
 */
std::future<void> getMetricsForInterval(const std::string& date, IntervalType intervalType);

/**
 * Get daily metrics (backward compatibility)
 * @deprecated Use getMetricsForInterval with 'day' interval instead
 */
std::future<void> getDailyMetrics(const std::string& date);

/**
 * Type definition for the interval metrics result
 */

/**
 * Type definition for the daily metrics result (backward compatibility)
 */

/**
 * Retrieves the content of a markdown summary file for a given date and interval type.
 * @param dateStr - The date string, formatted as YYYY-MM-DD for day/week, YYYY-MM for month.
 * @param intervalType - The type of interval (day, week, month).
 * @returns The markdown content as a string, or null if the file is not found or an error occurs.
 */
std::future<std::string> getIntervalSummaryContent(const std::string& dateStr, IntervalType intervalType);

} // namespace elizaos
