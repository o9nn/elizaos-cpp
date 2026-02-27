#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-NEXTJS-STARTER_SRC_TYPES_CHAT-MESSAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-NEXTJS-STARTER_SRC_TYPES_CHAT-MESSAGE_H
#include "core.h"

class ChatMessage;

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

    array<any> actions;
};

#endif
