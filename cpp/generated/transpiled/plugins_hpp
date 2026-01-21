#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_TYPES_PLUGINS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_TYPES_PLUGINS_H
#include "core.h"

typedef Record<string, string> RawRegistry;

class VersionInfo;
class CachedRegistry;

class VersionInfo : public object, public std::enable_shared_from_this<VersionInfo> {
public:
    using std::enable_shared_from_this<VersionInfo>::shared_from_this;
    object git;

    object npm;

    object supports;
};

class CachedRegistry : public object, public std::enable_shared_from_this<CachedRegistry> {
public:
    using std::enable_shared_from_this<CachedRegistry>::shared_from_this;
    string lastUpdatedAt;

    Record<string, std::shared_ptr<VersionInfo>> registry;
};

#endif
