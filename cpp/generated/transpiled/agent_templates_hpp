#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_CONFIG_AGENT-TEMPLATES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_CONFIG_AGENT-TEMPLATES_H
#include "core.h"
#include "@elizaos/core.h"

class AgentTemplate;

class AgentTemplate : public object, public std::enable_shared_from_this<AgentTemplate> {
public:
    using std::enable_shared_from_this<AgentTemplate>::shared_from_this;
    string id;

    string label;

    string description;

    Partial<std::shared_ptr<Agent>> template;
};

extern array<std::shared_ptr<AgentTemplate>> agentTemplates;
any getTemplateById(string id);

#endif
