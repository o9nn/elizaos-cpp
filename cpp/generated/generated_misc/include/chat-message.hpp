#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_TYPES_CHAT-MESSAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_TYPES_CHAT-MESSAGE_H
#include "core.h"

class ChatMessageAction;
class ChatMessage;

class ChatMessageAction : public object, public std::enable_shared_from_this<ChatMessageAction> {
public:
    using std::enable_shared_from_this<ChatMessageAction>::shared_from_this;
    std::string type;

    std::string content;

    Record<std::string, any> data;
};

class ChatMessage : public object, public std::enable_shared_from_this<ChatMessage> {
public:
    using std::enable_shared_from_this<ChatMessage>::shared_from_this;
    std::string id;

    std::string name;

    std::any text;

    std::string senderId;

    std::string roomId;

    double createdAt;

    std::string source;

    boolean isLoading;

    std::string thought;

    boolean isUserMessage;

    std::string serverMessageId;

    object content;

    array<std::shared_ptr<ChatMessageAction>> actions;
};

#endif
