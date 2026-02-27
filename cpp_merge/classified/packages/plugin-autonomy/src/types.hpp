#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTONOMY_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTONOMY_SRC_TYPES_H
#include "core.hpp"

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
