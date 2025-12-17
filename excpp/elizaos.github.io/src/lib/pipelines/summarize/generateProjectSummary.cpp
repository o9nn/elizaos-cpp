#include "generateProjectSummary.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> checkExistingSummary(const std::string& repoId, const std::string& date, IntervalType intervalType, std::optional<std::string> outputDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!outputDir) return false;

    const auto filename = std::to_string(date) + ".md";
    const auto summaryPath = getRepoFilePath(;
    outputDir,
    repoId,
    "summaries",
    intervalType,
    filename,
    );

    return existsSync(summaryPath);

}

} // namespace elizaos
