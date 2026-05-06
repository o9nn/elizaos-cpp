#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_NEXTJS_STARTER_SRC_TYPES_CHAT_MESSAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_NEXTJS_STARTER_SRC_TYPES_CHAT_MESSAGE_H
#include "core.hpp"

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
