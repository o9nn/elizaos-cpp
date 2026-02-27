#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_ACTIONS_UPDATEFORMAT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_ACTIONS_UPDATEFORMAT_H
#include "core.h"
#include "@elizaos/core.h"

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
