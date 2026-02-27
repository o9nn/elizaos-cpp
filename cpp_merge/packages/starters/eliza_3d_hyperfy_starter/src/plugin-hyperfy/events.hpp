#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_EVENTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_EVENTS_H
#include "core.hpp"
#include <string>
// External dependency removed
#include "./handlers/messageReceivedHandler.h"

enum struct hyperfyEventType;

enum struct hyperfyEventType {
    MESSAGE_RECEIVED = std::string("HYPERFY_MESSAGE_RECEIVED"), VOICE_MESSAGE_RECEIVED = std::string("HYPERFY_VOICE_MESSAGE_RECEIVED")
};
extern object hyperfyEvents;
#endif
