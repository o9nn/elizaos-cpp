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



struct ChatMessagesProps {
    std::vector<ChatMessageType> messages;
    std::unordered_map<double, std::vector<Citation>> citationsMap;
    std::unordered_map<double, std::vector<std::string>> followUpPromptsMap;
    (prompt: string) => void onFollowUpClick;
    std::optional<std::string> assistantAvatarUrl;
    std::optional<std::string> assistantName;
};


} // namespace elizaos
