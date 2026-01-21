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


/**
 * Security Warning Component for ELIZA Game
 *
 * Displays warnings for dangerous capabilities and requires
 * explicit user confirmation before enabling high-risk features
 */

struct SecurityWarningProps {
    std::string capability;
    std::string riskLevel;
    std::string description;
    std::vector<std::string> risks;
    bool isVisible;
};

/**
 * Security capability definitions for different plugins
 */

} // namespace elizaos
