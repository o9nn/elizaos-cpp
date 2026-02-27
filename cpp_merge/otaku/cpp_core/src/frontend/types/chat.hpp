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

struct ChatUser {
    std: id;
    std: name;
    std: username;
    std: avatar;
    std::optional<bool> isOnline;
};

struct ChatMessage {
    std: id;
    std: content;
    std: timestamp;
    std: senderId;
    bool isFromCurrentUser;
};

struct ChatConversation {
    std: id;
    std::vector<ChatUser> participants;
    ChatMessage lastMessage;
    double unreadCount;
    std::vector<ChatMessage> messages;
};

using ChatState = std:;

struct ChatData {
    ChatUser currentUser;
    std::vector<ChatConversation> conversations;
};


} // namespace elizaos
