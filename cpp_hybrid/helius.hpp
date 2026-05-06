#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_HELIUS_PROVIDERS_HELIUS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_HELIUS_PROVIDERS_HELIUS_H
#include "core.h"
#include "@elizaos/core.h"
#include "ws.h"
using WebSocket = _default;

class HeliusWebSocket;
class IToken;

class HeliusWebSocket : public object, public std::enable_shared_from_this<HeliusWebSocket> {
public:
    using std::enable_shared_from_this<HeliusWebSocket>::shared_from_this;
    string apiKey;

    std::shared_ptr<IAgentRuntime> runtime;

    any ws = nullptr;

    std::shared_ptr<Map<string, double>> subscriptions = std::make_shared<Map>();

    any pingInterval = nullptr;

    HeliusWebSocket(string apiKey_, std::shared_ptr<IAgentRuntime> runtime_);
    virtual std::shared_ptr<Promise<void>> connect();
    virtual std::shared_ptr<Promise<any>> subscribeToWallet(string walletAddress);
    virtual std::shared_ptr<Promise<boolean>> unsubscribeFromWallet(string walletAddress);
    virtual void startPing();
    virtual void cleanup();
    virtual void disconnect();
};

class IToken : public object, public std::enable_shared_from_this<IToken> {
public:
    using std::enable_shared_from_this<IToken>::shared_from_this;
    string symbol;

    string name;

    string address;

    double decimals;

    double totalSupply;

    double marketCap;

    double volume24h;

    double price;

    double price24hAgo;

    double priceChange24h;

    double curveProgress;
};

extern std::shared_ptr<Provider> heliusProvider;
#endif
