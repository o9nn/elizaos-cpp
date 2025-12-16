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

;
;
;
;
;

/**
 * Helper  = dateRange;
  let dateConditions: SQL[] = [];
  if (startDate && endDate) {
    // If startDate and endDate are the same, use equality
    if (startDate === endDate) {
      dateConditions = dateFields.map(
        (field) => sql`${table[field]} = ${startDate}`,
      );
    } else {
      // Otherwise use range with exclusive end date (<)
      dateConditions = dateFields.map(
        (field) =>
          sql`${table[field]} >= ${startDate} AND ${table[field]} < ${endDate}`,
      );
    }
  } else if (startDate) {
    dateConditions = dateFields.map(
      (field) => sql`${table[field]} >= ${startDate}`,
    );
  } else if (endDate) {
    dateConditions = dateFields.map(
      (field) => sql`${table[field]} < ${endDate}`,
    );
  }

  if (dateConditions.length === 0) {
    return [];
  }
  // At least one date field should match the range
  return [sql`(${or(...dateConditions)})`];
}

/**
 * Helper 

  if (params.repository && "repository" in table) {
    // Use properly parameterized sql query for repository
    conditions.push(sql`${table.repository} = ${params.repository}`);
  }

  return conditions;
}

/**
 * Get a date range for common time periods
 *
 * @param period - Time period identifier ('daily', 'weekly', 'monthly', 'quarterly', 'yearly', 'all')
 * @returns DateRange object with startDate and endDate strings
 */
;
  }

  const now = new UTCDate();
  const endDate = toDateString(now);
  let startDate: UTCDate;

  switch (period) {
    case "daily":
      // Just today
      startDate = now;
      break;
    case "weekly":
      // Start of current week (considering Sunday as first day)
      const day = now.getDay(); // 0 for Sunday, 1 for Monday, etc.
      startDate = new UTCDate(now);
      startDate.setDate(now.getDate() - day);
      break;
    case "monthly":
      // Start of current month
      startDate = new UTCDate(now.getFullYear(), now.getMonth(), 1);
      break;
    case "quarterly":
      // Start of current quarter
      const month = now.getMonth();
      const quarterStartMonth = Math.floor(month / 3) * 3;
      startDate = new UTCDate(now.getFullYear(), quarterStartMonth, 1);
      break;
    case "yearly":
      // Start of current year
      startDate = new UTCDate(now.getFullYear(), 0, 1);
      break;
    default:
      startDate = now;
  }

  return {
    startDate: toDateString(startDate),
    endDate,
  };
}

/**
 * Format a period identifier for time-based aggregations
 *
 * @param dateColumn - Date column to format
 * @param period - Aggregation period type
 * @returns SQL expression for the formatted period
 */
`;
    case "weekly":
      return sql<string>`strftime('%Y-W%W', ${dateColumn})`;
    case "monthly":
      return sql<string>`substr(${dateColumn}, 1, 7)`;
    case "quarterly":
      return sql<string>`
        substr(${dateColumn}, 1, 4) || '-Q' || 
        (cast(substr(${dateColumn}, 6, 2) as integer) + 2) / 3
      `;
    case "yearly":
      return sql<string>`substr(${dateColumn}, 1, 4)`;
    default:
      return sql<string>`${dateColumn}`;
  }
}

interface PaginatedQueryParams extends QueryParams {
  // Pagination
  limit?: number;
  offset?: number;
}

/**
 * Standard query parameters used across different queries
 */
struct QueryParams {
    std::optional<DateRange> dateRange;
    std::optional<std::string> repository;
};


} // namespace elizaos
