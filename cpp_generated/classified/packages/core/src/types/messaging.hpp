#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_MESSAGING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_MESSAGING_H
#include "core.h"
#include "./primitives.h"
#include "./runtime.h"

typedef std::function<std::shared_ptr<Promise<void>>(std::shared_ptr<IAgentRuntime>, std::shared_ptr<TargetInfo>, std::shared_ptr<Content>)> SendHandlerFunction;

class TargetInfo;
enum struct SOCKET_MESSAGE_TYPE;
class ControlMessage;

class TargetInfo : public object, public std::enable_shared_from_this<TargetInfo> {
public:
    using std::enable_shared_from_this<TargetInfo>::shared_from_this;
    string source;

    std::shared_ptr<UUID> roomId;

    string channelId;

    string serverId;

    std::shared_ptr<UUID> entityId;

    string threadId;
};

enum struct SOCKET_MESSAGE_TYPE {
    ROOM_JOINING = 1, SEND_MESSAGE = 2, MESSAGE = 3, ACK = 4, THINKING = 5, CONTROL = 6
};
class ControlMessage : public object, public std::enable_shared_from_this<ControlMessage> {
public:
    using std::enable_shared_from_this<ControlMessage>::shared_from_this;
    string type;

    object payload;

    std::shared_ptr<UUID> roomId;
};

#endif
