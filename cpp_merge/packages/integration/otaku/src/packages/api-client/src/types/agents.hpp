#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_AGENTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_AGENTS_H
#include "core.h"
#include "@elizaos/core.h"
#include "./base.h"

class Agent;
class AgentCreateParams;
class AgentUpdateParams;
class AgentWorld;
class AgentWorldSettings;
class AgentPanel;
class AgentLog;
class AgentLogsParams;

class Agent : public object, public std::enable_shared_from_this<Agent> {
public:
    using std::enable_shared_from_this<Agent>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    any bio;

    string characterName;

    any status;

    boolean enabled;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;

    Record<string, any> metadata;
};

class AgentCreateParams : public object, public std::enable_shared_from_this<AgentCreateParams> {
public:
    using std::enable_shared_from_this<AgentCreateParams>::shared_from_this;
    string characterPath;

    Record<string, any> characterJson;

    Record<string, any> agent;
};

class AgentUpdateParams : public object, public std::enable_shared_from_this<AgentUpdateParams> {
public:
    using std::enable_shared_from_this<AgentUpdateParams>::shared_from_this;
    string name;

    any bio;

    Record<string, any> metadata;
};

class AgentWorld : public object, public std::enable_shared_from_this<AgentWorld> {
public:
    using std::enable_shared_from_this<AgentWorld>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string description;

    array<std::shared_ptr<Agent>> agents;
};

class AgentWorldSettings : public object, public std::enable_shared_from_this<AgentWorldSettings> {
public:
    using std::enable_shared_from_this<AgentWorldSettings>::shared_from_this;
    std::shared_ptr<UUID> worldId;

    Record<string, any> settings;
};

class AgentPanel : public object, public std::enable_shared_from_this<AgentPanel> {
public:
    using std::enable_shared_from_this<AgentPanel>::shared_from_this;
    string id;

    string name;

    string url;

    string type;

    Record<string, any> metadata;
};

class AgentLog : public object, public std::enable_shared_from_this<AgentLog> {
public:
    using std::enable_shared_from_this<AgentLog>::shared_from_this;
    std::shared_ptr<UUID> id;

    string type;

    double timestamp;

    string message;

    string details;

    std::shared_ptr<UUID> roomId;

    object body;

    double createdAt;
};

class AgentLogsParams : public PaginationParams, public std::enable_shared_from_this<AgentLogsParams> {
public:
    using std::enable_shared_from_this<AgentLogsParams>::shared_from_this;
    any level;

    any from;

    any to;

    string search;
};

#endif
