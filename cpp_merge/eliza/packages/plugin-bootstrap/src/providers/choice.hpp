#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_BOOTSTRAP_SRC_PROVIDERS_CHOICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_BOOTSTRAP_SRC_PROVIDERS_CHOICE_H
#include "core.hpp"
// External dependency removed
// External dependency removed

class OptionObject;

class OptionObject : public object, public std::enable_shared_from_this<OptionObject> {
public:
    using std::enable_shared_from_this<OptionObject>::shared_from_this;
    string name;

    string description;
};

extern std::shared_ptr<Provider> choiceProvider;
#endif
