#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_TYPES_INDEX_H
#include "core.hpp"

class EnvVarConfig;

class EnvVarConfig : public object, public std::enable_shared_from_this<EnvVarConfig> {
public:
    using std::enable_shared_from_this<EnvVarConfig>::shared_from_this;
    string type;

    string description;

    boolean required;

    boolean sensitive;

    string default;
};

#endif
