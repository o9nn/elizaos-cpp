#include "generateContributorSummary.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> checkExistingSummary(const std::string& username, const std::variant<std::string, Date>& date, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto existingSummary = db.query.userSummaries.findFirst({;
        where: and(
        eq(userSummaries.username, username),
        eq(userSummaries.date, toDatestd::to_string(date)),
        eq(userSummaries.intervalType, intervalType),
        ),
        });

        return existingSummary != std::nullopt && existingSummary.summary != "";

}

} // namespace elizaos
