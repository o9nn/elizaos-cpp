#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_TYPES_SYSTEM_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_TYPES_SYSTEM_H
#include "core.h"

class SystemEnvironment;
class LocalEnvironmentUpdateParams;

class SystemEnvironment : public object, public std::enable_shared_from_this<SystemEnvironment> {
public:
    using std::enable_shared_from_this<SystemEnvironment>::shared_from_this;
    string nodeVersion;

    string platform;

    any environment;

    object features;

    Record<string, any> configuration;
};

class LocalEnvironmentUpdateParams : public object, public std::enable_shared_from_this<LocalEnvironmentUpdateParams> {
public:
    using std::enable_shared_from_this<LocalEnvironmentUpdateParams>::shared_from_this;
    Record<string, string> variables;

    boolean merge;
};

#endif
