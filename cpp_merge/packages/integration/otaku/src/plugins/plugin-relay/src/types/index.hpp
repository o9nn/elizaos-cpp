#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_RELAY_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_RELAY_SRC_TYPES_INDEX_H
#include "core.hpp"
#include "zod.hpp"
// External dependency removed

typedef any RelayChainId;

typedef any RelayCurrency;

typedef z::infer<QuoteRequestSchema> QuoteRequest;

typedef z::infer<BridgeRequestSchema> BridgeRequest;

typedef z::infer<ExecuteCallRequestSchema> ExecuteCallRequest;

typedef z::infer<StatusRequestSchema> StatusRequest;

class ResolvedBridgeRequest;
class RelayStatus;
class RelayExecuteResult;
class RelayChain;
class RelayCurrencyInfo;
class RelayPluginConfig;

extern object RelaySupportedChains;
extern any RelayCurrencies;
extern any QuoteRequestSchema;
extern any BridgeRequestSchema;
class ResolvedBridgeRequest : public object, public std::enable_shared_from_this<ResolvedBridgeRequest> {
public:
    using std::enable_shared_from_this<ResolvedBridgeRequest>::shared_from_this;
    string user;

    double originChainId;

    double destinationChainId;

    string currency;

    string toCurrency;

    string amount;

    string recipient;

    boolean useExactInput;

    boolean useExternalLiquidity;

    string referrer;
};

extern any ExecuteCallRequestSchema;
extern any StatusRequestSchema;
class RelayStatus : public object, public std::enable_shared_from_this<RelayStatus> {
public:
    using std::enable_shared_from_this<RelayStatus>::shared_from_this;
    string id;

    any status;

    string user;

    string recipient;

    string createdAt;

    string updatedAt;

    object data;
};

class RelayExecuteResult : public object, public std::enable_shared_from_this<RelayExecuteResult> {
public:
    using std::enable_shared_from_this<RelayExecuteResult>::shared_from_this;
    object data;

    string requestId;
};

class RelayChain : public object, public std::enable_shared_from_this<RelayChain> {
public:
    using std::enable_shared_from_this<RelayChain>::shared_from_this;
    double id;

    string name;

    string displayName;

    string httpRpcUrl;

    string wsRpcUrl;

    string explorerUrl;

    boolean depositEnabled;

    boolean withdrawEnabled;
};

class RelayCurrencyInfo : public object, public std::enable_shared_from_this<RelayCurrencyInfo> {
public:
    using std::enable_shared_from_this<RelayCurrencyInfo>::shared_from_this;
    object currency;

    double chainId;

    boolean depositEnabled;

    boolean withdrawEnabled;

    string minAmount;

    string maxAmount;
};

class RelayPluginConfig : public object, public std::enable_shared_from_this<RelayPluginConfig> {
public:
    using std::enable_shared_from_this<RelayPluginConfig>::shared_from_this;
    string RELAY_API_URL;

    string RELAY_API_KEY;

    string DEFAULT_SLIPPAGE;

    string MAX_PRICE_IMPACT;

    boolean ENABLE_TESTNET;
};

#endif
