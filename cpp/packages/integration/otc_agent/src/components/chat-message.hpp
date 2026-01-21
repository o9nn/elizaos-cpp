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



// Define constants if needed, or use literals directly

struct ChatMessageProps {
    ChatMessageType message;
    double i;
    std::optional<std::vector<Citation>> citations;
    std::optional<std::vector<std::string>> followUpPrompts;
    std::optional<std::string> assistantAvatarUrl;
    std::optional<std::string> assistantName;
};


} // namespace elizaos
