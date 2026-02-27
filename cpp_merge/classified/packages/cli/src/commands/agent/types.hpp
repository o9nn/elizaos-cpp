#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_AGENT_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_AGENT_TYPES_H
#include "core.hpp"

class AgentStartPayload;

class AgentStartPayload : public object, public std::enable_shared_from_this<AgentStartPayload> {
public:
    using std::enable_shared_from_this<AgentStartPayload>::shared_from_this;
    string characterPath;

    Record<string, any> characterJson;
};

#endif
