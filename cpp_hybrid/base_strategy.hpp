#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS-TRADER_STRATEGIES_BASE_STRATEGY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS-TRADER_STRATEGIES_BASE_STRATEGY_H
#include "core.h"
#include "@elizaos/core.h"

class strategy;
class strategy_wallet_config;

class strategy : public object, public std::enable_shared_from_this<strategy> {
public:
    using std::enable_shared_from_this<strategy>::shared_from_this;
};

class strategy_wallet_config : public object, public std::enable_shared_from_this<strategy_wallet_config> {
public:
    using std::enable_shared_from_this<strategy_wallet_config>::shared_from_this;
};

#endif
