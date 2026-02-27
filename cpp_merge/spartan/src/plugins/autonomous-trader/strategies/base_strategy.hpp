#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS_TRADER_STRATEGIES_BASE_STRATEGY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS_TRADER_STRATEGIES_BASE_STRATEGY_H
#include "core.hpp"
// External dependency removed

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
