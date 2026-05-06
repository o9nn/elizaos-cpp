#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "constants.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Represents the next unique identifier for an RPC request.
 */

/**
 * Represents the valid types that can be used for query parameters in a URL.
 * It can either be a key-value pair object with string, number, boolean, null or undefined values,
 * or an instance of the URLSearchParams class.
 */

/**
 * Interface representing retry options for a retry mechanism.
 * @typedef {Object} RetryOptions
 * @property {number} [maxRetries] - The maximum number of retries allowed.
 * @property {number} [initialDelay] - The initial delay in milliseconds before the first retry.
 * @property {number} [maxDelay] - The maximum delay in milliseconds between retries.
 * @property {number} [backoffFactor] - The factor by which the delay increases between retries.
 * @property {number[]} [retryableStatuses] - The array of HTTP status codes that are retryable.
 */
struct RetryOptions {
    std::optional<double> maxRetries;
    std::optional<double> initialDelay;
    std::optional<double> maxDelay;
    std::optional<double> backoffFactor;
    std::optional<std::vector<double>> retryableStatuses;
};

/**
 * Interface for defining options that can be passed in a request.
 * @template RequestOptions
 * @property {RetryOptions} [retryOptions] - Options for retrying the request
 * @property {QueryParams} [params] - Query parameters for the request
 */

/**
 * Represents an error that occurred during a request.
 * @extends Error
 * @param {string} message - The error message.
 * @param {Response} [response] - The response object associated with the error.
 */
class RequestError {
public:
    RequestError(const std::string& message);
};

/**
 * Build a URL with optional query parameters.
 *
 * @param {string} url - The base URL.
 * @param {QueryParams} [params] - Optional query parameters to be appended to the URL.
 */

/**
 * HTTP utility functions for making requests and handling responses.
 * @namespace http
 */

/**
 * Class representing a client for interacting with the Jupiter API for swapping tokens.
 */

class JupiterClient {
public:
    static void getQuote(const std::string& inputMint, const std::string& outputMint, const std::string& amount, auto slippageBps);
    static void swap(const std::any& quoteData, const std::string& walletPublicKey);
};

/**
 * Options for Dexscreener.
 * @typedef {Object} DexscreenerOptions
 * @property {string|CacheOptions["expires"]} [expires] - The expiration time for the cache.
 */
using DexscreenerOptions = {

/**
 * Client for interacting with DexScreener API.
 */

class DexscreenerClient {
public:
    DexscreenerClient();
    static void createFromRuntime(IAgentRuntime runtime);
    std::future<DexScreenerData> search(const std::string& address, std::optional<DexscreenerOptions> options);
    std::variant<Promise<DexScreenerPair, null>> searchForHighestLiquidityPair(const std::string& address, std::optional<std::string> chain, std::optional<DexscreenerOptions> options);
};

/**
 * Represents a client for interacting with the Helius API.
 */
class HeliusClient {
public:
    HeliusClient(IAgentRuntime _runtime);
    static void createFromRuntime(IAgentRuntime runtime);
    std::future<std::vector<HolderData>> fetchHolderList(const std::string& address, std::optional<std::any> options);

private:
    IAgentRuntime runtime_;
};

/**
 * Options for Coingecko API.
 * @typedef {Object} CoingeckoOptions
 * @property {string | CacheOptions["expires"]} [expires] - The expiration date for the cache.
 */
using CoingeckoOptions = {

/**
 * CoingeckoClient class for interacting with the Coingecko API.
 * @constructor
 * @param { string } apiKey - The API key required for accessing the Coingecko API.
 * @param { IAgentRuntime } runtime - The IAgentRuntime object for accessing runtime settings.
 */
class CoingeckoClient {
public:
    CoingeckoClient();
    static void createFromRuntime(IAgentRuntime runtime);
    std::future<Prices> fetchPrices(std::optional<CoingeckoOptions> options);
    void fetchGlobal();
    void fetchCategories();
};

/**
 * Represents an item in a wallet token list with details such as address, name, symbol, decimals, balance, UI amount, chain ID, logo URI, price in USD, and value in USD.
 * @typedef {Object} WalletTokenListItem
 * @property {string} address - The address of the token
 * @property {string} name - The name of the token
 * @property {string} symbol - The symbol of the token
 * @property {number} decimals - The decimals of the token
 * @property {number} balance - The balance of the token
 * @property {number} uiAmount - The UI amount of the token
 * @property {string} chainId - The chain ID of the token
 * @property {string} logoURI - The logo URI of the token
 * @property {number} priceUsd - The price of the token in USD
 * @property {number} valueUsd - The value of the token in USD
 */
using WalletTokenListItem = {

/**
 * Defines the structure of a WalletTokenList object, which includes the wallet name, total USD balance,
 * and an array of WalletTokenListItem objects.
 */
using WalletTokenList = {

/**
 * Represents a type that can either be "solana" or "ethereum" for the BirdeyeXChain.
 */
using BirdeyeXChain = std::string;

/**
 * Type representing headers for BirdeyeClient.
 * @typedef {Object} BirdeyeClientHeaders
 * @property {BirdeyeXChain} ["x-chain"] - Optional header for BirdeyeXChain.
 */
using BirdeyeClientHeaders = {

/**
 * Options for making a Birdeye API request.
 * @typedef {Object} BirdeyeRequestOptions
 * @property {BirdeyeXChain} [chain] - The BirdeyeX chain.
 * @property {string | CacheOptions["expires"]} [expires] - The expiration date for the request.
 */

using BirdeyeRequestOptions = {

/**
 * Class representing a client for interacting with the BirdEye API.
 */

class BirdeyeClient {
public:
    BirdeyeClient();
    static void createFromRuntime(IAgentRuntime runtime);
    std::future<double> fetchPrice(const std::string& address, std::optional<BirdeyeRequestOptions> options);
    std::future<Prices> fetchPrices();
    std::future<TokenOverview> fetchTokenOverview(const std::string& address, std::optional<BirdeyeRequestOptions> options, auto forceRefresh);
    std::future<TokenSecurityData> fetchTokenSecurity(const std::string& address, std::optional<BirdeyeRequestOptions> options);
    std::future<TokenTradeData> fetchTokenTradeData(const std::string& address, std::optional<BirdeyeRequestOptions> options);
    void fetchWalletTokenList(const std::string& address, std::optional<BirdeyeRequestOptions> options);
    std::future<WalletPortfolio> fetchPortfolioValue(const std::string& address, std::optional<BirdeyeRequestOptions> options);
};

/**
 * Parses a time string to milliseconds.
 * @param {string} timeStr - The time string to parse (e.g. '5ms', '10s').
 * @returns {number} The time string parsed to milliseconds, or 0 if the string cannot be parsed.
 */
void parseTimeToMs(const std::string& timeStr);

/**
 * Parses the expiration time to milliseconds.
 *
 * @param {string | number} expires - The expiration time to be parsed.
 * @returns {number} The expiration time in milliseconds.
 */
void parseExpires(const std::variant<std::string, double>& expires);

} // namespace elizaos
