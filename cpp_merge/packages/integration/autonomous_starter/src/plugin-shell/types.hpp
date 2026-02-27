#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SHELL_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SHELL_TYPES_H
#include "core.hpp"

namespace @elizaos/core {
    class ServiceTypeRegistry;
}

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string SHELL;
    };

}
extern object ShellServiceType, satisfies, Partial;
namespace @elizaos/core {
}
#endif
