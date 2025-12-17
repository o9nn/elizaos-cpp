#include "queryHelpers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

DateRange getDateRangeForPeriod(const std::string& period) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (period == "all") {
        return {}
    }

    const auto now = new UTCDate();
    const auto endDate = toDatestd::to_string(now);
    auto startDate: UTCDate;

    switch (period) {
        // case "daily":
        // Just today
        startDate = now;
        break;
        // case "weekly":
        // Start of current week (considering Sunday as first day)
        const auto day = now.getDay(); // 0 for Sunday, 1 for Monday, etc.;
        startDate = new UTCDate(now);
        startDate.setDate(now.getDate() - day);
        break;
        // case "monthly":
        // Start of current month
        startDate = new UTCDate(now.getFullYear(), now.getMonth(), 1);
        break;
        // case "quarterly":
        // Start of current quarter
        const auto month = now.getMonth();
        const auto quarterStartMonth = Math.floor(month / 3) * 3;
        startDate = new UTCDate(now.getFullYear(), quarterStartMonth, 1);
        break;
        // case "yearly":
        // Start of current year
        startDate = new UTCDate(now.getFullYear(), 0, 1);
        break;
        // default:
        startDate = now;
    }

    return {
        startDate: toDatestd::to_string(startDate),
        endDate,
        };

}

SQL<string> formatPeriodLabel(SQLiteColumn dateColumn, const std::string& period) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (period) {
        // case "daily":
        // Cast the column to SQL directly for the daily case
        return "sql<string>" + dateColumn;
        // case "weekly":
        return "sql<string>" + "strftime("%Y-W%W", " + dateColumn + ")";
        // case "monthly":
        return "sql<string>" + "substr(" + dateColumn + ", 1, 7)";
        // case "quarterly":
        return "sql<string>";
        substr(${dateColumn}, 1, 4) || "-Q" ||;
        (cast(substr(${dateColumn}, 6, 2)) + 2) / 3;
        `;
        // case "yearly":
        return "sql<string>" + "substr(" + dateColumn + ", 1, 4)";
        // default:
        return "sql<string>" + dateColumn;
    }

}

} // namespace elizaos
