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



// Custom remark plugin to remove the first H1

// Custom H2 component to apply primary color

struct SummaryContentProps {
    std::string summaryContent;
    std::optional<std::string> className;
};

void SummaryContent(auto className);

} // namespace elizaos
