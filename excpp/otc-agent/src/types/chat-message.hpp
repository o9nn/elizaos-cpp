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

struct ChatMessageAction {
    std::string type;
    std::optional<std::string> content;
};

struct ChatMessage {
    std::string id;
    std::string name;
    std::string text;
    std::string senderId;
    std::string roomId;
    double createdAt;
    std::string source;
    std::optional<bool> isLoading;
    std::optional<std::string> thought;
    std::optional<bool> isUserMessage;
    std::optional<std::string> serverMessageId;
    std::optional<std::vector<ChatMessageAction>> actions;
};


} // namespace elizaos
