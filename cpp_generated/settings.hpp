#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_SETTINGS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_SETTINGS_H
#include "core.h"

class RuntimeSettings;
class Setting;
class WorldSettings;
class OnboardingConfig;

class RuntimeSettings : public object, public std::enable_shared_from_this<RuntimeSettings> {
public:
    using std::enable_shared_from_this<RuntimeSettings>::shared_from_this;
};

class Setting : public object, public std::enable_shared_from_this<Setting> {
public:
    using std::enable_shared_from_this<Setting>::shared_from_this;
    string name;

    string description;

    string usageDescription;

    any value;

    boolean required;

    boolean public;

    boolean secret;

    std::function<boolean(any)> validation;

    array<string> dependsOn;

    std::function<string(any)> onSetAction;

    std::function<boolean(object)> visibleIf;
};

class WorldSettings : public object, public std::enable_shared_from_this<WorldSettings> {
public:
    using std::enable_shared_from_this<WorldSettings>::shared_from_this;
};

class OnboardingConfig : public object, public std::enable_shared_from_this<OnboardingConfig> {
public:
    using std::enable_shared_from_this<OnboardingConfig>::shared_from_this;
    object settings;
};

#endif
