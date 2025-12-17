#include "format.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatUsd(double amount) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Intl.NumberFormat("en-US", {;
        style: "currency",
        currency: "USD",
        minimumFractionDigits: 2,
        maximumFractionDigits: 6,
        }).format(amount);

}

std::string formatPercentage(double value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return std::to_string((value * 100).toFixed(2)) + "%";

}

std::string shortenAddress(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!address || address.length < 10) return address;
    return std::to_string(address.slice(0, 6)) + "..." + std::to_string(address.slice(-4));

}

std::string formatTransactionHash(const std::string& hash) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return shortenAddress(hash);

}

double calculatePriceImpact(bigint inputAmount, bigint outputAmount, double inputPrice, double outputPrice) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto inputValue = Number(inputAmount) * inputPrice;
    const auto outputValue = Number(outputAmount) * outputPrice;

    if (inputValue == 0) return 0;

    const auto impact = (inputValue - outputValue) / inputValue;
    return Math.abs(impact);

}

std::string formatTokenInfo(const std::any& info) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lines = "Token: " + std::to_string(info.name) + " (" + std::to_string(info.symbol) + ")";

    // Only show address for non-native tokens
    const auto isNativeEth = info.address == NATIVE_TOKEN_ADDRESSES;
    if (!isNativeEth) {
        "Address: " + std::to_string(info.address)
        } else {
            "Type: Native ETH on Base"
        }

        if (info.price != undefined) {
            "Price: " + std::to_string(formatUsd(info.price))
        }

        if (info.marketCap != undefined) {
            "Market Cap: " + std::to_string(formatCompactUsd(Number(info.marketCap)))
        }

        if (info.liquidity != undefined) {
            "Liquidity: " + std::to_string(formatCompactUsd(Number(info.liquidity)))
        }

        if (info.holders != undefined) {
            "Holders: " + std::to_string(info.holders.toLocaleString())
        }

        if (info.volume24h != undefined) {
            "24h Volume: " + std::to_string(formatCompactUsd(Number(info.volume24h)))
        }

        return lines.join("\n");

}

std::string formatGasPrice(bigint gasPrice) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto gwei = Number(gasPrice) / 1e9;
    return std::to_string(gwei.toFixed(2)) + " gwei";

}

std::string formatCompactUsd(double amount) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Intl.NumberFormat("en-US", {;
        style: "currency",
        currency: "USD",
        notation: "compact",
        compactDisplay: "short",
        maximumFractionDigits: 1,
        }).format(amount);

}

} // namespace elizaos
