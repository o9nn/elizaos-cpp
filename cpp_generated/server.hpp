#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_TYPES_SERVER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_TYPES_SERVER_H
#include "core.h"
#include "@elizaos/core.h"

class ServerHealth;
class ServerStatus;
class ServerDebugInfo;
class LogSubmitParams;

class ServerHealth : public object, public std::enable_shared_from_this<ServerHealth> {
public:
    using std::enable_shared_from_this<ServerHealth>::shared_from_this;
    any status;

    double uptime;

    std::shared_ptr<Date> timestamp;

    string version;

    Record<string, object> checks;
};

class ServerStatus : public object, public std::enable_shared_from_this<ServerStatus> {
public:
    using std::enable_shared_from_this<ServerStatus>::shared_from_this;
    object agents;

    object memory;

    double uptime;

    string version;
};

class ServerDebugInfo : public object, public std::enable_shared_from_this<ServerDebugInfo> {
public:
    using std::enable_shared_from_this<ServerDebugInfo>::shared_from_this;
    object runtime;

    Record<string, string> environment;
};

class LogSubmitParams : public object, public std::enable_shared_from_this<LogSubmitParams> {
public:
    using std::enable_shared_from_this<LogSubmitParams>::shared_from_this;
    any level;

    string message;

    string source;

    Record<string, any> metadata;
};

#endif
