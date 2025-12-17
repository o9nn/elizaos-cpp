#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct LlmCopyButtonProps {
    IntervalMetrics metrics;
    std::string summaryContent;
    std::optional<std::string> className;
};

void LlmCopyButton(auto summaryContent, auto className);

} // namespace elizaos
