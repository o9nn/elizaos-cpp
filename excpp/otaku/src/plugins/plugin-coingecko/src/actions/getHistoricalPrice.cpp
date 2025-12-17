#include "getHistoricalPrice.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatMarketCap(double value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (value >= 1000000000) return `${(value / 1000000000).toFixed(2)}B`;
    if (value >= 1000000) return `${(value / 1000000).toFixed(2)}M`;
    if (value >= 1000) return `${(value / 1000).toFixed(2)}K`;
    return value.toFixed(2);

}

std::string parseDateToApiFormat(const std::string& dateStr) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Try parsing various date formats and convert to dd-mm-yyyy
        const auto normalized = dateStr.trim().toLowerCase();

        // Check if already in dd-mm-yyyy format
        if (/^\d{2}-\d{2}-\d{4}$/.test(dateStr)) {
            return dateStr;
        }

        auto date: Date;

        // Parse common formats
        if (normalized == 'today') {
            date = new Date();
            } else if (normalized == "yesterday") {
                date = new Date();
                date.setDate(date.getDate() - 1);
                } else if (/^(\d+)\s*days?\s*ago$/.test(normalized)) {
                    const auto daysMatch = normalized.match(/^(\d+)\s*days?\s*ago$/);
                    const auto days = daysMatch ? parseInt(daysMatch[1]) : 0;
                    date = new Date();
                    date.setDate(date.getDate() - days);
                    } else if (/^(\d+)\s*weeks?\s*ago$/.test(normalized)) {
                        const auto weeksMatch = normalized.match(/^(\d+)\s*weeks?\s*ago$/);
                        const auto weeks = weeksMatch ? parseInt(weeksMatch[1]) : 0;
                        date = new Date();
                        date.setDate(date.getDate() - (weeks * 7));
                        } else if (/^(\d+)\s*months?\s*ago$/.test(normalized)) {
                            const auto monthsMatch = normalized.match(/^(\d+)\s*months?\s*ago$/);
                            const auto months = monthsMatch ? parseInt(monthsMatch[1]) : 0;
                            date = new Date();
                            date.setMonth(date.getMonth() - months);
                            } else if (/^(\d+)\s*years?\s*ago$/.test(normalized)) {
                                const auto yearsMatch = normalized.match(/^(\d+)\s*years?\s*ago$/);
                                const auto years = yearsMatch ? parseInt(yearsMatch[1]) : 0;
                                date = new Date();
                                date.setFullYear(date.getFullYear() - years);
                                } else {
                                    // Try parsing as a date string (yyyy-mm-dd, mm/dd/yyyy, etc.)
                                    date = new Date(dateStr);
                                    if (isNaN(date.getTime())) {
                                        throw std::runtime_error(`Unable to parse date: ${dateStr}`);
                                    }
                                }

                                // Convert to dd-mm-yyyy format
                                const auto day = std::to_string(date.getDate()).padStart(2, "0");
                                const auto month = std::to_string(date.getMonth() + 1).padStart(2, "0");
                                const auto year = date.getFullYear();

                                return day + "-" + month + "-" + year;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
