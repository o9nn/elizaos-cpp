#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_HOOKS_USE_SOL_PRICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_HOOKS_USE_SOL_PRICE_H
#include "core.hpp"
// External dependency removed
// External dependency removed

class SolPriceResponse;
class CoinGeckoResponse;
class BinanceResponse;

class SolPriceResponse : public object, public std::enable_shared_from_this<SolPriceResponse> {
public:
    using std::enable_shared_from_this<SolPriceResponse>::shared_from_this;
    double price;
};

class CoinGeckoResponse : public object, public std::enable_shared_from_this<CoinGeckoResponse> {
public:
    using std::enable_shared_from_this<CoinGeckoResponse>::shared_from_this;
    object solana;
};

class BinanceResponse : public object, public std::enable_shared_from_this<BinanceResponse> {
public:
    using std::enable_shared_from_this<BinanceResponse>::shared_from_this;
    string price;
};

extern std::function<std::shared_ptr<Promise<double>>()> fetchSolPrice;
any useSolPrice();

#endif
