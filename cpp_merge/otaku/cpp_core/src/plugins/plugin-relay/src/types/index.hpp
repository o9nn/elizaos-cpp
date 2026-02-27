#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Re-SDK types for convenience

// Supported chains for Relay

using RelayChainId = (typeof RelaySupportedChains)[keyof typeof RelaySupportedChains];

// Supported currencies

using RelayCurrency = (typeof RelayCurrencies)[number];

// Quote request schema - Updated for Relay SDK 2.x

using QuoteRequest = z.infer<typeof QuoteRequestSchema>;

// Note: Using SDK's Execute type as the quote response instead of custom RelayQuote

// Bridge request schema - LLM provides chain names, not IDs
// Note: user address is derived from EVM_PRIVATE_KEY, not from LLM

using BridgeRequest = z.infer<typeof BridgeRequestSchema>;

// Internal bridge request with resolved chain IDs
struct ResolvedBridgeRequest {
    std: user;
    double originChainId;
    double destinationChainId;
    std:;  // Contract address on origin chain currency;
    std::optional<std:;  // Contract address on destination chain> toCurrency;
    std:; // in wei amount;
    std::optional<std:> recipient;
    std::optional<bool> useExactInput;
    std::optional<bool> useExternalLiquidity;
    std::optional<std:> referrer;
};

// Execute call request schema

using ExecuteCallRequest = z.infer<typeof ExecuteCallRequestSchema>;

// Status request schema

using StatusRequest = z.infer<typeof StatusRequestSchema>;

// Transaction status
struct RelayStatus {
    std: id;
    "pending" | "success" | "failed" status;
    std: user;
    std: recipient;
    std: createdAt;
    std: updatedAt;
    { data;
    { fees;
    std::optional<std:> gas;
    std::optional<std:> relayer;
    Array<{ inTxs;
    std: hash;
    double chainId;
    double timestamp;
    Array<{ outTxs;
    std: hash;
    double chainId;
    double timestamp;
};

// Helper type for extracting request ID from Execute result
// The SDK's Execute type has a 'request' field which is an AxiosRequestConfig
// For our purposes, we need to check the actual result structure
struct RelayExecuteResult {
    std::optional<{> data;
    std::optional<{> request;
    std: id;
    std::optional<std:> requestId;
};

// Chain information
struct RelayChain {
    double id;
    std: name;
    std: displayName;
    std: httpRpcUrl;
    std::optional<std:> wsRpcUrl;
    std: explorerUrl;
    bool depositEnabled;
    bool withdrawEnabled;
};

// Currency information
struct RelayCurrencyInfo {
    { currency;
    std: contract;
    double decimals;
    std: name;
    std: symbol;
    double chainId;
    bool depositEnabled;
    bool withdrawEnabled;
    std: minAmount;
    std: maxAmount;
};

// Plugin configuration
struct RelayPluginConfig {
    std::optional<std:> RELAY_API_URL;
    std::optional<std:> RELAY_API_KEY;
    std::optional<std:> DEFAULT_SLIPPAGE;
    std::optional<std:> MAX_PRICE_IMPACT;
    std::optional<bool> ENABLE_TESTNET;
};


} // namespace elizaos
