#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_TYPES_H
#include "core.h"
#include "@solana/web3.js.h"

class Item;
class Prices;
class WalletPortfolio;
class TokenAccountInfo;

class Item : public object, public std::enable_shared_from_this<Item> {
public:
    using std::enable_shared_from_this<Item>::shared_from_this;
    string name;

    string address;

    string symbol;

    double decimals;

    string balance;

    string uiAmount;

    string priceUsd;

    string valueUsd;

    string valueSol;
};

class Prices : public object, public std::enable_shared_from_this<Prices> {
public:
    using std::enable_shared_from_this<Prices>::shared_from_this;
    object solana;

    object bitcoin;

    object ethereum;
};

class WalletPortfolio : public object, public std::enable_shared_from_this<WalletPortfolio> {
public:
    using std::enable_shared_from_this<WalletPortfolio>::shared_from_this;
    string totalUsd;

    string totalSol;

    array<std::shared_ptr<Item>> items;

    std::shared_ptr<Prices> prices;

    double lastUpdated;
};

class TokenAccountInfo : public object, public std::enable_shared_from_this<TokenAccountInfo> {
public:
    using std::enable_shared_from_this<TokenAccountInfo>::shared_from_this;
    std::shared_ptr<PublicKey> pubkey;

    object account;
};

#endif
