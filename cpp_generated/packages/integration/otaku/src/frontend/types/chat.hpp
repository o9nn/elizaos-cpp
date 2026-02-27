#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_FRONTEND_TYPES_CHAT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_FRONTEND_TYPES_CHAT_H
#include "core.h"

typedef any ChatState;

class ChatUser;
class ChatMessage;
class ChatConversation;
class ChatData;

class ChatUser : public object, public std::enable_shared_from_this<ChatUser> {
public:
    using std::enable_shared_from_this<ChatUser>::shared_from_this;
    string id;

    string name;

    string username;

    string avatar;

    boolean isOnline;
};

class ChatMessage : public object, public std::enable_shared_from_this<ChatMessage> {
public:
    using std::enable_shared_from_this<ChatMessage>::shared_from_this;
    string id;

    string content;

    string timestamp;

    string senderId;

    boolean isFromCurrentUser;
};

class ChatConversation : public object, public std::enable_shared_from_this<ChatConversation> {
public:
    using std::enable_shared_from_this<ChatConversation>::shared_from_this;
    string id;

    array<std::shared_ptr<ChatUser>> participants;

    std::shared_ptr<ChatMessage> lastMessage;

    double unreadCount;

    array<std::shared_ptr<ChatMessage>> messages;
};

class ChatData : public object, public std::enable_shared_from_this<ChatData> {
public:
    using std::enable_shared_from_this<ChatData>::shared_from_this;
    std::shared_ptr<ChatUser> currentUser;

    array<std::shared_ptr<ChatConversation>> conversations;
};

#endif
