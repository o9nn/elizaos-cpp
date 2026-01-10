#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_TYPES_SHARED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_TYPES_SHARED_H
#include "core.h"
#include "../utils.h"

typedef any BirdeyeSupportedChain;

class BaseAddress;
class WalletAddress;
class TokenAddress;
class ContractAddress;

class BaseAddress : public object, public std::enable_shared_from_this<BaseAddress> {
public:
    using std::enable_shared_from_this<BaseAddress>::shared_from_this;
    any type;

    string symbol;

    string address;

    BirdeyeSupportedChain chain;
};

class WalletAddress : public BaseAddress, public std::enable_shared_from_this<WalletAddress> {
public:
    using std::enable_shared_from_this<WalletAddress>::shared_from_this;
    string type;
};

class TokenAddress : public BaseAddress, public std::enable_shared_from_this<TokenAddress> {
public:
    using std::enable_shared_from_this<TokenAddress>::shared_from_this;
    string type;
};

class ContractAddress : public BaseAddress, public std::enable_shared_from_this<ContractAddress> {
public:
    using std::enable_shared_from_this<ContractAddress>::shared_from_this;
    string type;
};

#endif
