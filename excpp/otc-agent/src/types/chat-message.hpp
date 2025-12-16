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

struct ChatMessageAction {
    std::string type;
    std::optional<std::string> content;
    std::optional<std::unordered_map<std::string, unknown>> data;
};


struct ChatMessage {
    std::string id;
    std::string name;
    string | null | undefined text;
    std::string senderId;
    std::string roomId;
    double createdAt;
    std::string source;
    std::optional<bool> isLoading;
    std::optional<std::string> thought;
    std::optional<bool> isUserMessage;
    std::optional<std::string> serverMessageId;
    std::optional<{ text?: string }; // For messages that have structured content> content;
    std::optional<std::vector<ChatMessageAction>> actions;
};


} // namespace elizaos
