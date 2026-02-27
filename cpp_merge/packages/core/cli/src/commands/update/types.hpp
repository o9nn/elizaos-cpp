#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_UPDATE_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_UPDATE_TYPES_H
#include "core.hpp"

class UpdateOptions;
class GlobalUpdateOptions;
class VersionCheckResult;
class PackageUpdate;
class UpdateCheckResult;
class CliEnvironment;
class UpdateContext;

class UpdateOptions : public object, public std::enable_shared_from_this<UpdateOptions> {
public:
    using std::enable_shared_from_this<UpdateOptions>::shared_from_this;
    boolean dryRun;

    boolean skipBuild;

    boolean skipBunMigration;
};

class GlobalUpdateOptions : public object, public std::enable_shared_from_this<GlobalUpdateOptions> {
public:
    using std::enable_shared_from_this<GlobalUpdateOptions>::shared_from_this;
    string version;

    boolean skipBunMigration;
};

class VersionCheckResult : public object, public std::enable_shared_from_this<VersionCheckResult> {
public:
    using std::enable_shared_from_this<VersionCheckResult>::shared_from_this;
    boolean needsUpdate;

    string error;
};

class PackageUpdate : public object, public std::enable_shared_from_this<PackageUpdate> {
public:
    using std::enable_shared_from_this<PackageUpdate>::shared_from_this;
    string current;

    string latest;
};

class UpdateCheckResult : public object, public std::enable_shared_from_this<UpdateCheckResult> {
public:
    using std::enable_shared_from_this<UpdateCheckResult>::shared_from_this;
    boolean hasUpdates;

    Record<string, std::shared_ptr<PackageUpdate>> updates;
};

class CliEnvironment : public object, public std::enable_shared_from_this<CliEnvironment> {
public:
    using std::enable_shared_from_this<CliEnvironment>::shared_from_this;
    boolean isGlobal;

    boolean isNpx;

    boolean isBunx;

    boolean isNpmInstalled;

    string packageManager;
};

class UpdateContext : public object, public std::enable_shared_from_this<UpdateContext> {
public:
    using std::enable_shared_from_this<UpdateContext>::shared_from_this;
    string cwd;

    boolean isPlugin;

    any directoryInfo;

    std::shared_ptr<CliEnvironment> environment;
};

#endif
