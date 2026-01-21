#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_ACTIONS_UPDATEFORMAT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_ACTIONS_UPDATEFORMAT_H
#include "core.h"
#include "@elizaos/core.h"

class TeamMember;

class TeamMember : public object, public std::enable_shared_from_this<TeamMember> {
public:
    using std::enable_shared_from_this<TeamMember>::shared_from_this;
    std::string section;

    std::string tgName;

    std::string discordName;

    std::string format;

    std::string serverId;

    std::string serverName;

    std::string createdAt;

    array<string> updatesFormat;
};

std::shared_ptr<UUID> getStorageRoomId(std::shared_ptr<IAgentRuntime> runtime, std::string serverId);

extern std::shared_ptr<Action> updatesFormatAction;
#endif
