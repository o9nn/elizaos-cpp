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



struct ChatMessagesProps {
    std::vector<ChatMessageType> messages;
    std::optional<std::string> assistantAvatarUrl;
    std::optional<std::string> assistantName;
};


} // namespace elizaos
