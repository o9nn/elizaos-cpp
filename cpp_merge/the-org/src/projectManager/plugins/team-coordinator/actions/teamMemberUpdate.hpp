#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_ACTIONS_TEAMMEMBERUPDATE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_ACTIONS_TEAMMEMBERUPDATE_H
#include "core.hpp"
// External dependency removed
#include "../../../types.h"

class IDiscordService;
class ReportChannelConfig;

class IDiscordService : public Service, public std::enable_shared_from_this<IDiscordService> {
public:
    using std::enable_shared_from_this<IDiscordService>::shared_from_this;
    object client;
};

class ReportChannelConfig : public object, public std::enable_shared_from_this<ReportChannelConfig> {
public:
    using std::enable_shared_from_this<ReportChannelConfig>::shared_from_this;
    string serverId;

    string channelId;
};

std::shared_ptr<Promise<boolean>> postUpdateToDiscordChannel(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<TeamMemberUpdate> update);

std::shared_ptr<Promise<boolean>> storeTeamMemberUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<TeamMemberUpdate> update);

std::shared_ptr<Promise<any>> parseTeamMemberUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message);

extern std::shared_ptr<Action> teamMemberUpdatesAction;
#endif
