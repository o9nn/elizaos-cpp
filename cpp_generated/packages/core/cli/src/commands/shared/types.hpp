#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_SHARED_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_SHARED_TYPES_H
#include "core.h"

template <typename T>
class ApiResponse;
class AgentBasic;

template <typename T>
class ApiResponse : public object, public std::enable_shared_from_this<ApiResponse<T>> {
public:
    using std::enable_shared_from_this<ApiResponse<T>>::shared_from_this;
    boolean success;

    T data;

    object error;
};

class AgentBasic : public object, public std::enable_shared_from_this<AgentBasic> {
public:
    using std::enable_shared_from_this<AgentBasic>::shared_from_this;
    string id;

    string name;

    string status;
};

#endif
