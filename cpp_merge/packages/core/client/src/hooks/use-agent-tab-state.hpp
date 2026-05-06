#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE_AGENT_TAB_STATE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE_AGENT_TAB_STATE_H
#include "core.hpp"
#include "react.hpp"
// External dependency removed
// External dependency removed
// Using alias removed (invalid transpilation)

typedef any TabValue;

class AgentTabStates;

extern string AGENT_TAB_STATE_KEY;
class AgentTabStates : public object, public std::enable_shared_from_this<AgentTabStates> {
public:
    using std::enable_shared_from_this<AgentTabStates>::shared_from_this;
};

any useAgentTabState(any agentId);

#endif
