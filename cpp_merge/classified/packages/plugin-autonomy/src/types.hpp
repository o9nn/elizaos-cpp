#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTONOMY_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTONOMY_SRC_TYPES_H
#include "core.h"

namespace @elizaos/core {
    class ServiceTypeRegistry;
}

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string AUTONOMOUS;
    };

}
extern object AutonomousServiceType, satisfies, Partial;
namespace @elizaos/core {
}
#endif
