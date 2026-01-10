#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_PLUGIN-HYPERFY_EVENTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_PLUGIN-HYPERFY_EVENTS_H
#include "core.h"
#include "@elizaos/core.h"
#include "./handlers/messageReceivedHandler.h"

enum struct hyperfyEventType;

enum struct hyperfyEventType {
    MESSAGE_RECEIVED = std::string("HYPERFY_MESSAGE_RECEIVED"), VOICE_MESSAGE_RECEIVED = std::string("HYPERFY_VOICE_MESSAGE_RECEIVED")
};
extern object hyperfyEvents;
#endif
