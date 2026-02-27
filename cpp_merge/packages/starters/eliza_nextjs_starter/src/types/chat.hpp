#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_NEXTJS_STARTER_SRC_TYPES_CHAT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_NEXTJS_STARTER_SRC_TYPES_CHAT_H
#include "core.hpp"
#include "ai.hpp"

class Citation;
class ChatStreamData;
class ChatRequest;
class ChatResponse;

class Citation : public object, public std::enable_shared_from_this<Citation> {
public:
    using std::enable_shared_from_this<Citation>::shared_from_this;
    string url;

    string content;

    string title;
};

class ChatStreamData : public object, public std::enable_shared_from_this<ChatStreamData> {
public:
    using std::enable_shared_from_this<ChatStreamData>::shared_from_this;
    array<std::shared_ptr<Citation>> citations;
};

class ChatRequest : public object, public std::enable_shared_from_this<ChatRequest> {
public:
    using std::enable_shared_from_this<ChatRequest>::shared_from_this;
    array<std::shared_ptr<Message>> messages;
};

class ChatResponse : public ChatStreamData, public std::enable_shared_from_this<ChatResponse> {
public:
    using std::enable_shared_from_this<ChatResponse>::shared_from_this;
    string id;

    array<std::shared_ptr<Message>> messages;

    array<string> followUpPrompts;
};

#endif
