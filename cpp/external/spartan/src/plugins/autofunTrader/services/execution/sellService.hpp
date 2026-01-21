#include "..idl/autofun.json.hpp"
#include "..types.hpp"
#include "..types/autofun.hpp"
#include "..utils/bignumber.hpp"
#include "..utils/wallet.hpp"
#include ".analyticsService.hpp"
#include ".base/BaseTradeService.hpp"
#include ".calculation/tradeCalculation.hpp"
#include ".dataService.hpp"
#include ".tradeMemoryService.hpp"
#include ".validation/TokenValidationService.hpp"
#include ".walletService.hpp"
#include "elizaos/core.hpp"
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



// Define ISellSignalOutput interface
struct ISellSignalOutput {
    std::string recommended_sell;
    std::string recommend_sell_address;
    std::optional<string; // Or reason_for_selling, adjust based on actual LLM output> reason;
    std::optional<string; // Alternative for reason> reason_for_selling;
    string | number sell_amount;
    std::optional<number | string> slippage;
};

// Move TradeExecutionResult to the top level and it
struct TradeExecutionResult {
    bool success;
    std::optional<std::string> signature;
    std::optional<std::string> receivedAmount;
    std::optional<std::string> receivedValue;
    std::optional<std::string> error;
};

double convertToBasisPoints(double feePercent);

struct ConfigAccount {
    PublicKey teamWallet;
    AnchorBN platformSellFee;
    AnchorBN platformBuyFee;
};

double calculateAmountOutBuy(double reserveToken, double amount, double _solDecimals, double reserveLamport, double platformBuyFee);

/**
 * Calculates the amount of SOL received when selling tokens
 */
double calculateAmountOutSell(double reserveLamport, double amount, double _tokenDecimals, double platformSellFee, double reserveToken);

  // Apply platform fee

  // Calculate expected output
    // Buy
    // Sell

  // Apply slippage to estimated output

  // Apply the fee instruction to the transaction

class SellService extends BaseTradeService {
  private pendingSells: { [tokenAddress: string]: BN } = {};

  // https://github.com/elizaOS/auto.fun/blob/7b9c4e6a38ff93c882a87198388e5381a3d40a7a/packages/client/src/utils/swapUtils.ts#L37
  // https://github.com/elizaOS/auto.fun/blob/7b9c4e6a38ff93c882a87198388e5381a3d40a7a/packages/client/src/hooks/use-swap.ts#L3
    // Replace the cache lookup with direct wallet balance check
    //console.log('walletData', walletData)
    //console.log('tokenData', tokenData)

    // autofun program autoUmixaMaYKFjexMpQuBpNYntgbkzCo2b1ZqUaAZ5

    // t.id for the ca

    // inject into prompt

    // get balance from plugin-solana

    // Get sell recommendation from model

    // Validate token address format

    // Await the getWallet() call

    //console.log('token', token)

    /*

    */

    //.times(10 ** tokenBalance.decimals)

      /*
      */
      /*
      */

    // for anchor

    // Use the imported IDL for typing, cast to any to bypass potential strict type mismatch

    // is this right?

    // Get fresh blockhash with processed commitment for speed

    // Send transaction

        // Add validation for slippage with warning and enforce stricter limits
        /*

        */

        // why are we getting this after the trade execution?
        // for the price? shouldn't we already have it?
        //console.log('sell marketData', marketData)


} // namespace elizaos
