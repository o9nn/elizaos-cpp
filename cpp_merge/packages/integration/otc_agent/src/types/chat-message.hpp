#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_TYPES_CHAT_MESSAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_TYPES_CHAT_MESSAGE_H
#include "core.hpp"

class ChatMessageAction;
class ChatMessage;

class ChatMessageAction : public object, public std::enable_shared_from_this<ChatMessageAction> {
public:
    using std::enable_shared_from_this<ChatMessageAction>::shared_from_this;
    string type;

    string content;

    Record<string, any> data;
};

class ChatMessage : public object, public std::enable_shared_from_this<ChatMessage> {
public:
    using std::enable_shared_from_this<ChatMessage>::shared_from_this;
    string id;

    string name;

    any text;

    string senderId;

    string roomId;

    double createdAt;

    string source;

    boolean isLoading;

    string thought;

    boolean isUserMessage;

    string serverMessageId;

    object content;

    array<std::shared_ptr<ChatMessageAction>> actions;
};

#endif
