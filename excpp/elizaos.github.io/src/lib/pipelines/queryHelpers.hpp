#include ".data/types.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Helper function to build date range conditions for different date fields
 */

    // If startDate and endDate are the same, use equality
      // Otherwise use range with exclusive end date (<)

  // At least one date field should match the range

/**
 * Helper function to build common where conditions based on query params
 */

    // Use properly parameterized sql query for repository

/**
 * Get a date range for common time periods
 *
 * @param period - Time period identifier ('daily', 'weekly', 'monthly', 'quarterly', 'yearly', 'all')
 * @returns DateRange object with startDate and endDate strings
 */
DateRange getDateRangeForPeriod("all" | "daily" | "weekly" | "monthly" | "quarterly" | "yearly" period);

/**
 * Format a period identifier for time-based aggregations
 *
 * @param dateColumn - Date column to format
 * @param period - Aggregation period type
 * @returns SQL expression for the formatted period
 */
SQL<string> formatPeriodLabel(SQLiteColumn dateColumn, "daily" | "weekly" | "monthly" | "quarterly" | "yearly" period);

  // Pagination

/**
 * Standard query parameters used across different queries
 */
struct QueryParams {
    std::optional<DateRange> dateRange;
    std::optional<std::string> repository;
};


} // namespace elizaos
