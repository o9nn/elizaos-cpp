#include "elizaos/core.hpp"
#include <future>
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



/**
 * Token resolution strategy copied from CDP service resolver.
 */
struct TokenMetadata {
    std: symbol;
    std: name;
    std: address;
    double decimals;
    std: network;
};

struct CoinGeckoTokenResponse {
    std::optional<std:> symbol;
    std::optional<std:> name;
    std::optional<std::unordered_map<std:, std:>> platforms;
    std::optional<std::unordered_map<std:, { decimal_place? }>> detail_platforms;

struct CoinGeckoSearchCoin {
    std: id;
    std: symbol;
    std: name;
};

struct CoinGeckoSearchResponse {
    std::optional<std::vector<CoinGeckoSearchCoin>> coins;
};

struct CoinGeckoCoinDetailResponse {
    std::optional<std::unordered_map<std:, std:>> platforms;
};

std: getPlatformId(const std:& network);

std: getCacheKey(const std:& network, const std:& address);

bool isCacheValid(const std:& key);

std::future<TokenMetadata | null> getTokenMetadata(const std:& address, const std:& network);

std::future<std: | null> resolveTokenSymbol(const std:& symbol, const std:& network);

std::future<"0x> resolveTokenToAddress(const std:& token, const std:& network);" | null> {
  
  // Handle native gas tokens
  // EXCEPTION: On Polygon, ETH refers to WETH (bridged ETH), not the native gas token
  
  // Handle MATIC/POL on Polygon (native gas token)
  
  // Handle MATIC on other chains (would be wrapped/bridged MATIC)
    // Fall through to normal resolution
  
  // Handle POL on other chains (ERC20 token on Ethereum, not native gas token)
  // POL exists as ERC20 on Ethereum mainnet, but is NOT a native gas token anywhere except Polygon
    // Fall through to normal resolution (will resolve to ERC20 contract address)

std::future<double> getTokenDecimals(const std:& address, const std:& network);

void clearTokenCache();

void getCacheStats(); {

void addHardcodedTokenAddress(const std:& network, const std:& symbol, const std:& address);

std::unordered_map<std:, std:> getHardcodedTokens(const std:& network);

} // namespace elizaos
