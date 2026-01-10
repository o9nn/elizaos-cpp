#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TASKS_SELLSIGNAL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TASKS_SELLSIGNAL_H
#include "core.h"
#include "@elizaos/core.h"
#include "../schemas.h"
#include "../types.h"
#include "../../degenTrader/types.h"
#include "../../degenTrader/utils/wallet.h"
#include "../../degenTrader/types.h"
#include "../../degenTrader/types/trading.h"

class ISellSignalOutput;
class SellSignal;

extern string rolePrompt;
extern string template;
class ISellSignalOutput : public object, public std::enable_shared_from_this<ISellSignalOutput> {
public:
    using std::enable_shared_from_this<ISellSignalOutput>::shared_from_this;
    string recommended_sell;

    string recommend_sell_address;

    double marketcap;

    string reason;

    string sell_amount;
};

class SellSignal : public object, public std::enable_shared_from_this<SellSignal> {
public:
    using std::enable_shared_from_this<SellSignal>::shared_from_this;
    string apiKey;

    std::shared_ptr<IAgentRuntime> runtime;

    SellSignal(std::shared_ptr<IAgentRuntime> runtime);
    virtual std::shared_ptr<Promise<boolean>> generateSignal();
    virtual any getBalance();
};

using _default = SellSignal;
#endif
