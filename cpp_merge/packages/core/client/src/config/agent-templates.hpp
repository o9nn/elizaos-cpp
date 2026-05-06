#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_CONFIG_AGENT_TEMPLATES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_CONFIG_AGENT_TEMPLATES_H
#include "core.hpp"
// External dependency removed

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
