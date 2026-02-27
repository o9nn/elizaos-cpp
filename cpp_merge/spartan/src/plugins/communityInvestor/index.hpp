#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_INDEX_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "./service.h"
#include "./tests.h"
#include "./routes.h"
#include "./events.h"

class AgentPanel;

class AgentPanel : public object, public std::enable_shared_from_this<AgentPanel> {
public:
    using std::enable_shared_from_this<AgentPanel>::shared_from_this;
    string name;

    string path;

    string component;

    string icon;

    boolean public;
};

extern std::shared_ptr<Plugin> communityInvestorPlugin;
extern array<std::shared_ptr<AgentPanel>> panels;
#endif
