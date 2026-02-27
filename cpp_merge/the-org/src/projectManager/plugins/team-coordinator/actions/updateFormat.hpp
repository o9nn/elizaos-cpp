#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_ACTIONS_UPDATEFORMAT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_ACTIONS_UPDATEFORMAT_H
#include "core.hpp"
// External dependency removed

class TeamMember;

class TeamMember : public object, public std::enable_shared_from_this<TeamMember> {
public:
    using std::enable_shared_from_this<TeamMember>::shared_from_this;
    string section;

    string tgName;

    string discordName;

    string format;

    string serverId;

    string serverName;

    string createdAt;

    array<string> updatesFormat;
};

std::shared_ptr<UUID> getStorageRoomId(std::shared_ptr<IAgentRuntime> runtime, string serverId);

extern std::shared_ptr<Action> updatesFormatAction;
#endif
