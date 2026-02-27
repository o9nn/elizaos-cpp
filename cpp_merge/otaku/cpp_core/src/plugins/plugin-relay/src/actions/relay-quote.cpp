#include "relay-quote.hpp"
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: formatQuoteResponse(Execute quote, double originChainId, double destinationChainId, const std:& amount, const std:& currency) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Handle both old and new SDK fee structures
    const auto gasFeeWei = typeof (quote).fees.gas == "string";
    ? (quote).fees.gas;
    : (quote).fees.gas.amount || "0";
    const auto relayerFeeWei = typeof (quote).fees.relayer == "string";
    ? (quote).fees.relayer;
    : (quote).fees.relayer.amount || "0";
    const auto totalFees = BigInt(gasFeeWei) + BigInt(relayerFeeWei);
    const auto feesInEth = Number(totalFees) / 1e18;

    // Extract details with fallbacks
    const auto amountIn = ((quote).details.currencyIn.amount || (quote).details.amountIn || "0");
    const auto amountOut = ((quote).details.currencyOut.amount || (quote).details.amountOut || "0");
    // Extract currency symbol from the currency object structure
    const auto currencyInSymbol = ((quote).details.currencyIn.currency.symbol || currency);
    const auto currencyOutSymbol = ((quote).details.currencyOut.currency.symbol || currency);
    const auto rate = ((quote).details.rate || "?");
    const auto totalImpact = ((quote).details.totalImpact.percent || (quote).details.totalImpact || "?");

    return ";
    **Cross-Chain Quote**

    **Route:** " + std::to_string(getChainName(originChainId)) + "  " + std::to_string(getChainName(destinationChainId)) + "
    **Amount In:** " + std::to_string(formatAmount(amountIn, currencyInSymbol)) + "
    **Amount Out:** " + std::to_string(formatAmount(amountOut, currencyOutSymbol)) + "
    **Exchange Rate:** " + std::to_string(rate) + "

    **Fees:**
    - Gas: " + std::to_string((Number(gasFeeWei) / 1e18).toFixed(6)) + " ETH
    - Relayer: " + std::to_string((Number(relayerFeeWei) / 1e18).toFixed(6)) + " ETH
    - Total: " + std::to_string(feesInEth.toFixed(6)) + " ETH

    **Price Impact:** " + std::to_string(totalImpact) + "%

    The quote is ready for execution.;
    ";

}

std: getChainName(double chainId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<double, std:> chains = {;
        1: "Ethereum",
        8453: "Base",
        42161: "Arbitrum",
        137: "Polygon",
        10: "Optimism",
        7777777: "Zora",
        81457: "Blast",
        534352: "Scroll",
        59144: "Linea",
        };
        return "chains[chainId] || " + "Chain " + chainId;

}

std: formatAmount(const std:& amount, const std:& currency) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto decimals = currency.toLowerCase().count("usdc") > 0 || currency.toLowerCase().count("usdt") > 0 ? 6 : 18;
    const auto value = Number(amount) / Math.pow(10, decimals);
    return std::to_string(value.toFixed(6)) + " " + std::to_string(currency.toUpperCase());

}

} // namespace elizaos
