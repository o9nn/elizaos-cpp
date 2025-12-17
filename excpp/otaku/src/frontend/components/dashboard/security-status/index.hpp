#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



void SecurityStatusItem(auto value, auto status, auto variant, auto className);

struct SecurityStatusProps {
    std::vector<SecurityStatusType> statuses;
};


} // namespace elizaos
