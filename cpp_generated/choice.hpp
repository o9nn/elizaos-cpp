#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-BOOTSTRAP_PROVIDERS_CHOICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-BOOTSTRAP_PROVIDERS_CHOICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"

class OptionObject;

class OptionObject : public object, public std::enable_shared_from_this<OptionObject> {
public:
    using std::enable_shared_from_this<OptionObject>::shared_from_this;
    string name;

    string description;
};

extern std::shared_ptr<Provider> choiceProvider;
#endif
