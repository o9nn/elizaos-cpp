#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE-AGENT-TAB-STATE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE-AGENT-TAB-STATE_H
#include "core.h"
#include "react.h"
#include "@elizaos/core.h"
#include "@/lib/logger.h"
using clientLogger = _default;

typedef any TabValue;

class AgentTabStates;

extern string AGENT_TAB_STATE_KEY;
class AgentTabStates : public object, public std::enable_shared_from_this<AgentTabStates> {
public:
    using std::enable_shared_from_this<AgentTabStates>::shared_from_this;
};

any useAgentTabState(any agentId);

#endif
