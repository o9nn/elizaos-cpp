#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Token resolution strategy copied from CDP service resolver.
 */
struct TokenMetadata {
    std::string symbol;
    std::string name;
    std::string address;
    double decimals;
    std::string network;
};

struct CoinGeckoTokenResponse {
    std::optional<std::string> symbol;
    std::optional<std::string> name;
};

struct CoinGeckoSearchCoin {
    std::string id;
    std::string symbol;
    std::string name;
};

struct CoinGeckoSearchResponse {
    std::optional<std::vector<CoinGeckoSearchCoin>> coins;
};

struct CoinGeckoCoinDetailResponse {
};

std::string getPlatformId(const std::string& network);

std::string getCacheKey(const std::string& network, const std::string& address);

bool isCacheValid(const std::string& key);

std::future<std::string> resolveTokenSymbol(const std::string& symbol, const std::string& network);

std::future<`0x> resolveTokenToAddress(const std::string& token, const std::string& network);` | null> {
  
  // Handle native gas tokens
  // EXCEPTION: On Polygon, ETH refers to WETH (bridged ETH), not the native gas token
  
  // Handle MATIC/POL on Polygon (native gas token)
  
  // Handle MATIC on other chains (would be wrapped/bridged MATIC)
    // Fall through to normal resolution
  
  // Handle POL on other chains (ERC20 token on Ethereum, not native gas token)
  // POL exists as ERC20 on Ethereum mainnet, but is NOT a native gas token anywhere except Polygon
    // Fall through to normal resolution (will resolve to ERC20 contract address)

std::future<double> getTokenDecimals(const std::string& address, const std::string& network);

void clearTokenCache();


void addHardcodedTokenAddress(const std::string& network, const std::string& symbol, const std::string& address);

std::unordered_map<std::string, std::string> getHardcodedTokens(const std::string& network);

} // namespace elizaos
