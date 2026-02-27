#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_BIRDEYE_SRC_TYPES_API_WALLET_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_BIRDEYE_SRC_TYPES_API_WALLET_H
#include "core.hpp"

class WalletPortfolioParams;
class WalletPortfolioResponse;
class WalletTokenBalanceParams;
class WalletTokenBalanceResponse;
class WalletTransactionHistoryParams;
class WalletTransactionHistoryResponse;
class WalletNetworksResponse;
class WalletPortfolioMultichainParams;
class WalletPortfolioMultichainResponse;
class WalletTransactionHistoryMultichainParams;
class WalletTransactionHistoryMultichainResponse;
class WalletSimulationParams;
class WalletSimulationResponse;

class WalletPortfolioParams : public object, public std::enable_shared_from_this<WalletPortfolioParams> {
public:
    using std::enable_shared_from_this<WalletPortfolioParams>::shared_from_this;
    string wallet;
};

class WalletPortfolioResponse : public object, public std::enable_shared_from_this<WalletPortfolioResponse> {
public:
    using std::enable_shared_from_this<WalletPortfolioResponse>::shared_from_this;
    boolean success;

    object data;
};

class WalletTokenBalanceParams : public object, public std::enable_shared_from_this<WalletTokenBalanceParams> {
public:
    using std::enable_shared_from_this<WalletTokenBalanceParams>::shared_from_this;
    string wallet;

    string token_address;
};

class WalletTokenBalanceResponse : public object, public std::enable_shared_from_this<WalletTokenBalanceResponse> {
public:
    using std::enable_shared_from_this<WalletTokenBalanceResponse>::shared_from_this;
    boolean success;

    object data;
};

class WalletTransactionHistoryParams : public object, public std::enable_shared_from_this<WalletTransactionHistoryParams> {
public:
    using std::enable_shared_from_this<WalletTransactionHistoryParams>::shared_from_this;
    string wallet;

    double limit;

    string before;
};

class WalletTransactionHistoryResponse : public object, public std::enable_shared_from_this<WalletTransactionHistoryResponse> {
public:
    using std::enable_shared_from_this<WalletTransactionHistoryResponse>::shared_from_this;
    boolean success;

    object data;
};

class WalletNetworksResponse : public object, public std::enable_shared_from_this<WalletNetworksResponse> {
public:
    using std::enable_shared_from_this<WalletNetworksResponse>::shared_from_this;
    boolean success;

    object data;
};

class WalletPortfolioMultichainParams : public object, public std::enable_shared_from_this<WalletPortfolioMultichainParams> {
public:
    using std::enable_shared_from_this<WalletPortfolioMultichainParams>::shared_from_this;
    string wallet;
};

class WalletPortfolioMultichainResponse : public object, public std::enable_shared_from_this<WalletPortfolioMultichainResponse> {
public:
    using std::enable_shared_from_this<WalletPortfolioMultichainResponse>::shared_from_this;
    boolean success;

    object data;
};

class WalletTransactionHistoryMultichainParams : public object, public std::enable_shared_from_this<WalletTransactionHistoryMultichainParams> {
public:
    using std::enable_shared_from_this<WalletTransactionHistoryMultichainParams>::shared_from_this;
    string wallet;
};

class WalletTransactionHistoryMultichainResponse : public object, public std::enable_shared_from_this<WalletTransactionHistoryMultichainResponse> {
public:
    using std::enable_shared_from_this<WalletTransactionHistoryMultichainResponse>::shared_from_this;
    boolean success;

    object data;
};

class WalletSimulationParams : public object, public std::enable_shared_from_this<WalletSimulationParams> {
public:
    using std::enable_shared_from_this<WalletSimulationParams>::shared_from_this;
    string from;

    string to;

    string data;

    string value;
};

class WalletSimulationResponse : public object, public std::enable_shared_from_this<WalletSimulationResponse> {
public:
    using std::enable_shared_from_this<WalletSimulationResponse>::shared_from_this;
    boolean success;

    object data;
};

#endif
