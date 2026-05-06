#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_SYSTEM_VERSION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_SYSTEM_VERSION_H
#include "core.h"
#include "express.h"
using express = _default;
#include "../../../package.json.h"
using packageJson = _default;

class VersionInfo;

class VersionInfo : public object, public std::enable_shared_from_this<VersionInfo> {
public:
    using std::enable_shared_from_this<VersionInfo>::shared_from_this;
    string version;

    string source;

    string timestamp;

    string environment;

    double uptime;

    string error;
};

std::shared_ptr<VersionInfo> getVersionInfo();

std::shared_ptr<express::Router> createVersionRouter();

#endif
