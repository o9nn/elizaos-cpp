#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC___TESTS___PROVIDER_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC___TESTS___PROVIDER_TEST_H
#include "core.h"
#include "vitest.h"
#include "../provider.h"
#include "../state.h"
#include "../state.h"
#include "../provider.h"
#include "@elizaos/core-plugin-v2.h"

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
