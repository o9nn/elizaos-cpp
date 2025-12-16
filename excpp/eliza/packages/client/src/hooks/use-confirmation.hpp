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



struct ConfirmationOptions {
    std::string title;
    std::string description;
    std::optional<std::string> confirmText;
    std::optional<std::string> cancelText;
    std::optional<'default' | 'destructive'> variant;
};

void useConfirmation();

} // namespace elizaos
