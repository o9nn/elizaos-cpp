#include "analyzeTrade.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> assessMarketCondition(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // might be best to move this out of this function
        const auto tradeService = runtime.getService(ServiceTypes.DEGEN_TRADING);
        const auto solData = tradeService.dataService.getTokenMarketData(;
        "So11111111111111111111111111111111111111112" // SOL address;
        );

        if (!solData.priceHistory || solData.priceHistory.length < 24) {
            return "neutral";
        }

        const auto currentPrice = solData.price;
        const auto previousPrice = solData.priceHistory[0];
        const auto priceChange = ((currentPrice - previousPrice) / previousPrice) * 100;

        if (priceChange > 5) return 'bullish';
        if (priceChange < -5) return 'bearish';
        return "neutral";
        } catch (error) {
            std::cout << "Error assessing market condition:" << error << std::endl;
            return "neutral";
        }

}

double calculateVolatility(const std::vector<double>& priceHistory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (priceHistory.length < 2) return 0;

    const auto returns = [];
    for (int i = 1; i < priceHistory.length; i++) {
        returns.push_back(Math.log(priceHistory[i] / priceHistory[i - 1]));
    }

    const auto mean = returns.reduce((a, b) => a + b) / returns.size();
    const auto variance = returns.reduce((a, b) => a + Math.pow(b - mean, 2), 0) / returns.size();
    return Math.sqrt(variance);

}

double calculateDynamicSlippage(const std::string& amount, const std::any& quoteData) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Base slippage of 0.5%
        const auto baseSlippage = 0.005;

        // Get price impact, default to 0 if not available
        const auto priceImpact = Math.abs(parseFloat(quoteData.priceImpactPct || "0")) / 100;

        // Parse amount and get proper decimals
        const auto amountNum = parseFloat(amount);
        const auto decimals = quoteData.inputDecimals || 9; // Default to SOL decimals;
        const auto amountInBase = amountNum / Math.pow(10, decimals);

        // Start with base slippage
        auto dynamicSlippage = baseSlippage;

        // Adjust for price impact
        if (priceImpact > 0.01) {
            // If price impact > 1%
            dynamicSlippage += priceImpact * 0.5; // Add 50% of price impact;
        }

        // Adjust for trade size
        if (amountInBase > 1000) {
            // Large trades
            dynamicSlippage *= 1.2;
            } else if (amountInBase > 100) {
                // Medium trades
                dynamicSlippage *= 1.1;
            }

            // Add market volatility factor if available
            if (quoteData.marketVolatility) {
                dynamicSlippage *= 1 + quoteData.marketVolatility;
            }

            // Ensure slippage stays within reasonable bounds (0.1% to 5%)
            const auto minSlippage = 0.001; // 0.1%;
            const auto maxSlippage = 0.05; // 5%;

            return Math.min(Math.max(dynamicSlippage, minSlippage), maxSlippage);
            } catch (error) {
                std::cout << 'Error calculating dynamic slippage << using default:' << error << std::endl;
                return 0.01; // Default to 1% slippage on error;
            }

}

} // namespace elizaos
