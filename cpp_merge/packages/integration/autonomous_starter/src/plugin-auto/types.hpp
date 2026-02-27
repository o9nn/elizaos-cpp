#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-AUTO_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-AUTO_TYPES_H
#include "core.h"

namespace @elizaos/core {
    class ServiceTypeRegistry;
}
enum struct EventType;

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string AUTONOMOUS;
    };

}
extern object AutonomousServiceType, satisfies, Partial;
enum struct EventType {
    AUTO_MESSAGE_RECEIVED = std:("auto_message_received")
};
namespace @elizaos/core {
}
#endif
