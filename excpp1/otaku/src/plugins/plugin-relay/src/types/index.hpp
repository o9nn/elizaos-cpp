#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Re-SDK types for convenience

// Supported chains for Relay

using RelayChainId = (typeof RelaySupportedChains)[keyof typeof RelaySupportedChains];

// Supported currencies

using RelayCurrency = (typeof RelayCurrencies)[number];

// Quote request schema - Updated for Relay SDK 2.x

using QuoteRequest = z::infer<typeof QuoteRequestSchema>;

// Note: Using SDK's Execute type as the quote response instead of custom RelayQuote

// Bridge request schema - LLM provides chain names, not IDs
// Note: user address is derived from EVM_PRIVATE_KEY, not from LLM

using BridgeRequest = z::infer<typeof BridgeRequestSchema>;

// Internal bridge request with resolved chain IDs
struct ResolvedBridgeRequest {
    std::string user;
    double originChainId;
    double destinationChainId;
    std::string currency;
    std::optional<std::string> toCurrency;
    std::string amount;
    std::optional<std::string> recipient;
    std::optional<bool> useExactInput;
    std::optional<bool> useExternalLiquidity;
    std::optional<std::string> referrer;
};

// Execute call request schema

using ExecuteCallRequest = z::infer<typeof ExecuteCallRequestSchema>;

// Status request schema

using StatusRequest = z::infer<typeof StatusRequestSchema>;

// Transaction status
struct RelayStatus {
    std::string id;
    std::variant<"pending", "success", "failed"> status;
    std::string user;
    std::string recipient;
    std::string createdAt;
    std::string updatedAt;
    std::optional<std::string> gas;
    std::optional<std::string> relayer;
    std::string hash;
    double chainId;
    double timestamp;
    std::string hash;
    double chainId;
    double timestamp;
};

// Helper type for extracting request ID from Execute result
// The SDK's Execute type has a 'request' field which is an AxiosRequestConfig
// For our purposes, we need to check the actual result structure
struct RelayExecuteResult {
    std::string id;
    std::optional<std::string> requestId;
};

// Chain information
struct RelayChain {
    double id;
    std::string name;
    std::string displayName;
    std::string httpRpcUrl;
    std::optional<std::string> wsRpcUrl;
    std::string explorerUrl;
    bool depositEnabled;
    bool withdrawEnabled;
};

// Currency information
struct RelayCurrencyInfo {
    std::string contract;
    double decimals;
    std::string name;
    std::string symbol;
    double chainId;
    bool depositEnabled;
    bool withdrawEnabled;
    std::string minAmount;
    std::string maxAmount;
};

// Plugin configuration
struct RelayPluginConfig {
    std::optional<std::string> RELAY_API_URL;
    std::optional<std::string> RELAY_API_KEY;
    std::optional<std::string> DEFAULT_SLIPPAGE;
    std::optional<std::string> MAX_PRICE_IMPACT;
    std::optional<bool> ENABLE_TESTNET;
};


} // namespace elizaos
