#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_DEV_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_DEV_TYPES_H
#include "core.hpp"
#include "node:child_process.h"

class DevOptions;
class ServerProcess;
class WatcherConfig;
class DevContext;
class FileChangeEvent;
class BuildResult;

class DevOptions : public object, public std::enable_shared_from_this<DevOptions> {
public:
    using std::enable_shared_from_this<DevOptions>::shared_from_this;
    double port;

    string configDir;

    array<string> character;

    boolean configure;
};

class ServerProcess : public object, public std::enable_shared_from_this<ServerProcess> {
public:
    using std::enable_shared_from_this<ServerProcess>::shared_from_this;
    any process;

    virtual std::shared_ptr<Promise<void>> stop() = 0;
    virtual std::shared_ptr<Promise<void>> start(array<string> args = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> restart(array<string> args = undefined) = 0;
};

class WatcherConfig : public object, public std::enable_shared_from_this<WatcherConfig> {
public:
    using std::enable_shared_from_this<WatcherConfig>::shared_from_this;
    array<string> ignored;

    boolean ignoreInitial;

    boolean persistent;

    boolean followSymlinks;

    double depth;

    boolean usePolling;

    double interval;
};

class DevContext : public object, public std::enable_shared_from_this<DevContext> {
public:
    using std::enable_shared_from_this<DevContext>::shared_from_this;
    string directory;

    any directoryType;

    string watchDirectory;

    boolean buildRequired;
};

class FileChangeEvent : public object, public std::enable_shared_from_this<FileChangeEvent> {
public:
    using std::enable_shared_from_this<FileChangeEvent>::shared_from_this;
    any type;

    string path;

    any stats;
};

class BuildResult : public object, public std::enable_shared_from_this<BuildResult> {
public:
    using std::enable_shared_from_this<BuildResult>::shared_from_this;
    boolean success;

    double duration;

    std::shared_ptr<Error> error;
};

#endif
