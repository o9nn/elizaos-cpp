#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TASKS_TSK_TRENDING-SIGNAL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TASKS_TSK_TRENDING-SIGNAL_H
#include "core.h"
#include "@elizaos/core.h"
#include "../schemas.h"
#include "../types.h"

class IBuySignalOutput;
class BuySignal;

extern string DEGEN_WALLET;
extern string _rolePrompt;
extern string _template;
class IBuySignalOutput : public object, public std::enable_shared_from_this<IBuySignalOutput> {
public:
    using std::enable_shared_from_this<IBuySignalOutput>::shared_from_this;
    string recommended_buy;

    string recommend_buy_address;

    double marketcap;

    string reason;

    string buy_amount;
};

class BuySignal : public object, public std::enable_shared_from_this<BuySignal> {
public:
    using std::enable_shared_from_this<BuySignal>::shared_from_this;
    string apiKey;

    std::shared_ptr<IAgentRuntime> runtime;

    BuySignal(std::shared_ptr<IAgentRuntime> runtime);
    virtual std::shared_ptr<Promise<boolean>> generateSignal();
    virtual any getBalance();
};

using _default = BuySignal;
#endif
