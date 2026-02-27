#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC___TESTS___PROVIDER_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC___TESTS___PROVIDER_TEST_H
#include "core.hpp"
#include "vitest.hpp"
#include "../provider.h"
#include "../state.h"
#include "../state.h"
#include "../provider.h"
// External dependency removed

class ProviderV2;

class ProviderV2 : public object, public std::enable_shared_from_this<ProviderV2> {
public:
    using std::enable_shared_from_this<ProviderV2>::shared_from_this;
    string name;

    string description;

    boolean dynamic;

    double position;

    boolean private;

    std::function<std::shared_ptr<Promise<any>>(any, any, any)> get;
};

extern any mockRuntime;
extern any mockMessage;
#endif
