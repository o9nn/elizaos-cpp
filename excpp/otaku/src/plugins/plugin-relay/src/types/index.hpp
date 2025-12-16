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

;
import type { 
  Execute, 
  ExecuteStep, 
  ExecuteStepItem,
  ProgressData,
  CallFees,
  CallBreakdown,
  QuoteDetails,
} from "@relayprotocol/relay-sdk";

// Re-SDK types for convenience
type { Execute, ExecuteStep, ExecuteStepItem, ProgressData, CallFees, CallBreakdown, QuoteDetails };

// Supported chains for Relay
const RelaySupportedChains = {
  ETHEREUM: 1,
  BASE: 8453,
  ARBITRUM: 42161,
  POLYGON: 137,
  OPTIMISM: 10,
  ZORA: 7777777,
  BLAST: 81457,
  SCROLL: 534352,
  LINEA: 59144,
} as const;

using RelayChainId = (typeof RelaySupportedChains)[keyof typeof RelaySupportedChains];

// Supported currencies
const RelayCurrencies = [
  "eth",
  "usdc",
  "usdt",
  "weth",
  "usdc.e",
  "wbtc",
  "degen",
  "tia",
] as const;

using RelayCurrency = (typeof RelayCurrencies)[number];

// Quote request schema - Updated for Relay SDK 2.x
const QuoteRequestSchema = z.object({
  user: z.string().describe("User wallet address"),
  chainId: z.number().describe("Origin chain ID"),  // renamed from originChainId
  toChainId: z.number().describe("Destination chain ID"),  // renamed from destinationChainId
  currency: z.string().describe("Currency on origin chain"),  // renamed from originCurrency
  toCurrency: z.string().optional().describe("Currency on destination chain"),  // renamed from destinationCurrency
  amount: z.string().describe("Amount in wei"),
  recipient: z.string().optional().describe("Recipient address (defaults to user)"),
  tradeType: z.enum(["EXACT_INPUT", "EXACT_OUTPUT"]).optional().default("EXACT_INPUT"),
  referrer: z.string().optional().describe("Referrer address for fees"),
});

using QuoteRequest = z.infer<typeof QuoteRequestSchema>;

// Note: Using SDK's Execute type as the quote response instead of custom RelayQuote

// Bridge request schema - LLM provides chain names, not IDs
// Note: user address is derived from EVM_PRIVATE_KEY, not from LLM
const BridgeRequestSchema = z.object({
  originChain: z.string().describe("Origin chain name (e.g., 'ethereum', 'base', 'arbitrum')"),
  destinationChain: z.string().describe("Destination chain name (e.g., 'ethereum', 'base', 'arbitrum')"),
  currency: z.string().describe("Currency to bridge (symbol like 'eth', 'usdc')"),
  amount: z.string().describe("Amount in human-readable format (e.g., '0.5' for 0.5 ETH)"),
  recipient: z.string().optional().describe("Recipient address (defaults to user's wallet)"),
  useExactInput: z.boolean().optional().default(true),
  useExternalLiquidity: z.boolean().optional().default(false),
  referrer: z.string().optional().describe("Referrer address"),
});

using BridgeRequest = z.infer<typeof BridgeRequestSchema>;

// Internal bridge request with resolved chain IDs
struct ResolvedBridgeRequest {
    std::string user;
    double originChainId;
    double destinationChainId;
    string;  // Contract address on origin chain currency;
    std::optional<string;  // Contract address on destination chain> toCurrency;
    string; // in wei amount;
    std::optional<std::string> recipient;
    std::optional<bool> useExactInput;
    std::optional<bool> useExternalLiquidity;
    std::optional<std::string> referrer;
};


// Execute call request schema
const ExecuteCallRequestSchema = z.object({
  user: z.string().describe("User wallet address"),
  originChainId: z.number().describe("Origin chain ID"),
  destinationChainId: z.number().describe("Destination chain ID"),
  originCurrency: z.string().describe("Currency on origin chain"),
  amount: z.string().describe("Amount in wei"),
  txs: z.array(
    z.object({
      to: z.string().describe("Contract address to call"),
      value: z.string().describe("Value to send"),
      data: z.string().describe("Calldata"),
    })
  ).describe("Array of transactions to execute on destination"),
  recipient: z.string().optional().describe("Recipient address"),
});

using ExecuteCallRequest = z.infer<typeof ExecuteCallRequestSchema>;

// Status request schema
const StatusRequestSchema = z.object({
  requestId: z.string().optional().describe("Request ID to check status"),
  txHash: z.string().optional().describe("Transaction hash to check status"),
  user: z.string().optional().describe("Filter by user address"),
});

using StatusRequest = z.infer<typeof StatusRequestSchema>;

// Transaction status
struct RelayStatus {
    std::string id;
    "pending" | "success" | "failed" status;
    std::string user;
    std::string recipient;
    std::string createdAt;
    std::string updatedAt;
    { data;
    { fees;
    std::optional<std::string> gas;
    std::optional<std::string> relayer;
    Array<{ inTxs;
    std::string hash;
    double chainId;
    double timestamp;
    Array<{ outTxs;
    std::string hash;
    double chainId;
    double timestamp;
};


// Helper type for extracting request ID from Execute result
// The SDK's Execute type has a 'request' field which is an AxiosRequestConfig
// For our purposes, we need to check the actual result structure
struct RelayExecuteResult {
    std::optional<{> data;
    std::optional<{> request;
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
    { currency;
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
