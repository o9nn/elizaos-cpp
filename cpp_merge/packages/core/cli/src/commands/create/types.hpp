#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_TYPES_H
#include "core.hpp"
#include "zod.hpp"

typedef z::infer<initOptionsSchema> CreateOptions;

class AIModelOption;
class DatabaseOption;

extern any initOptionsSchema;
class AIModelOption : public object, public std::enable_shared_from_this<AIModelOption> {
public:
    using std::enable_shared_from_this<AIModelOption>::shared_from_this;
    string title;

    string value;

    string description;
};

class DatabaseOption : public object, public std::enable_shared_from_this<DatabaseOption> {
public:
    using std::enable_shared_from_this<DatabaseOption>::shared_from_this;
    string title;

    string value;

    string description;
};

#endif
