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

// Enhanced with StickToBottom for better UX

struct ScrollState {
    bool isAtBottom;
    bool autoScrollEnabled;
};

struct UseAutoScrollOptions {
    std::optional<double> offset;
    std::optional<bool> smooth;
};

void useAutoScroll(UseAutoScrollOptions = {} options);

} // namespace elizaos
