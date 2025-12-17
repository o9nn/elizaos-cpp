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



struct ChatMessagesProps {
    std::vector<ChatMessageType> messages;
    std::optional<std::string> assistantAvatarUrl;
    std::optional<std::string> assistantName;
};

void ChatMessages(auto citationsMap, auto followUpPromptsMap, auto onFollowUpClick, auto assistantAvatarUrl, auto assistantName);

} // namespace elizaos
