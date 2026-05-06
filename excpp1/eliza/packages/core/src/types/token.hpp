#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * A standardized representation of a token holding.
 */
struct TokenBalance {
    std::string address;
    std::string balance;
    double decimals;
    std::optional<double> uiAmount;
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<std::string> logoURI;
};

/**
 * Generic representation of token data that can be provided by various services.
 */
struct TokenData {
    std::string id;
    std::string symbol;
    std::string name;
    std::string address;
    std::string chain;
    std::string sourceProvider;
    std::optional<double> price;
    std::optional<double> priceChange24hPercent;
    std::optional<double> priceChange24hUSD;
    std::optional<double> volume24hUSD;
    std::optional<double> marketCapUSD;
    std::optional<double> liquidity;
    std::optional<double> holders;
    std::optional<std::string> logoURI;
    std::optional<double> decimals;
    std::optional<Date> lastUpdatedAt;
    std::optional<std::any> raw;
};

/**
 * Interface for a generic service that provides token data.
 */


} // namespace elizaos
