#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_MONOREPO_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_MONOREPO_TYPES_H
#include "core.h"

class MonorepoOptions;
class CloneInfo;
class PlatformInstructions;

class MonorepoOptions : public object, public std::enable_shared_from_this<MonorepoOptions> {
public:
    using std::enable_shared_from_this<MonorepoOptions>::shared_from_this;
    string branch;

    string dir;
};

class CloneInfo : public object, public std::enable_shared_from_this<CloneInfo> {
public:
    using std::enable_shared_from_this<CloneInfo>::shared_from_this;
    string repo;

    string branch;

    string destination;
};

class PlatformInstructions : public object, public std::enable_shared_from_this<PlatformInstructions> {
public:
    using std::enable_shared_from_this<PlatformInstructions>::shared_from_this;
    string platform;

    array<string> commands;

    array<string> alternatives;
};

#endif
