#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_ENV_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_ENV_TYPES_H
#include "core.hpp"

typedef any ResetTarget;

typedef any ResetAction;

typedef Record<string, string> EnvVars;

typedef Record<ResetAction, array<string>> ResetActionRecord;

class ListEnvOptions;
class EditEnvOptions;
class ResetEnvOptions;
class InteractiveEnvOptions;
class ResetItem;

class ListEnvOptions : public object, public std::enable_shared_from_this<ListEnvOptions> {
public:
    using std::enable_shared_from_this<ListEnvOptions>::shared_from_this;
    boolean system;

    boolean local;
};

class EditEnvOptions : public object, public std::enable_shared_from_this<EditEnvOptions> {
public:
    using std::enable_shared_from_this<EditEnvOptions>::shared_from_this;
    boolean yes;
};

class ResetEnvOptions : public object, public std::enable_shared_from_this<ResetEnvOptions> {
public:
    using std::enable_shared_from_this<ResetEnvOptions>::shared_from_this;
    boolean yes;
};

class InteractiveEnvOptions : public object, public std::enable_shared_from_this<InteractiveEnvOptions> {
public:
    using std::enable_shared_from_this<InteractiveEnvOptions>::shared_from_this;
    boolean yes;
};

class ResetItem : public object, public std::enable_shared_from_this<ResetItem> {
public:
    using std::enable_shared_from_this<ResetItem>::shared_from_this;
    string title;

    ResetTarget value;

    string description;

    boolean selected;
};

#endif
