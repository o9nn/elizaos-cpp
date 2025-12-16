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
 * Security Warning Component for ELIZA Game
 *
 * Displays warnings for dangerous capabilities and requires
 * explicit user confirmation before enabling high-risk features
 */

struct SecurityWarningProps {
    std::string capability;
    'low' | 'medium' | 'high' | 'critical' riskLevel;
    std::string description;
    std::vector<std::string> risks;
    () => void onConfirm;
    () => void onCancel;
    bool isVisible;
};

/**
 * Security capability definitions for different plugins
 */

} // namespace elizaos
