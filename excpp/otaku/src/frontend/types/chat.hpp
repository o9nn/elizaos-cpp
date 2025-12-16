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

struct ChatUser {
    std::string id;
    std::string name;
    std::string username;
    std::string avatar;
    std::optional<bool> isOnline;
};


struct ChatMessage {
    std::string id;
    std::string content;
    std::string timestamp;
    std::string senderId;
    bool isFromCurrentUser;
};


struct ChatConversation {
    std::string id;
    std::vector<ChatUser> participants;
    ChatMessage lastMessage;
    double unreadCount;
    std::vector<ChatMessage> messages;
};


using ChatState = std::variant<"collapsed", "expanded", "conversation">;

struct ChatData {
    ChatUser currentUser;
    std::vector<ChatConversation> conversations;
};


} // namespace elizaos
