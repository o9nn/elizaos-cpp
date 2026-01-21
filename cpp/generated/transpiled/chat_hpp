#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_TYPES_CHAT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_TYPES_CHAT_H
#include "core.h"

class Citation;
class ChatStreamData;

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

#endif
