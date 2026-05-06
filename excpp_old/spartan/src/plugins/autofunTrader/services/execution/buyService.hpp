#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "...degenTrader/utils/wallet.hpp"
#include "..idl/autofun.json.hpp"
#include "..types.hpp"
#include "..types/autofun.hpp"
#include "..utils/bignumber.hpp"
#include ".analyticsService.hpp"
#include ".base/BaseTradeService.hpp"
#include ".calculation/tradeCalculation.hpp"
#include ".dataService.hpp"
#include ".tradeMemoryService.hpp"
#include ".validation/TokenValidationService.hpp"
#include ".walletService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct IBuySignalOutput {
    std::string recommended_buy;
    std::string recommend_buy_address;
    std::string reason;
    std::variant<std::string, double> buy_amount;
};

struct ConfigAccount {
    PublicKey teamWallet;
    BN platformSellFee;
    BN platformBuyFee;
};

double convertToBasisPoints(double feePercent);

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

// Only choose a token that occurs in both the Trending Tokens list as well as the Sentiment analysis. This ensures we have the proper token address.
// The sentiment score has a range of -100 to 100, with -100 indicating extreme negativity and 100 indicating extreme positiveness.

class BuyService {
public:
    BuyService(IAgentRuntime runtime, WalletService walletService, DataService dataService, AnalyticsService analyticsService, TradeMemoryService tradeMemoryService);
    std::future<void> initialize();
    std::future<void> stop();
    void generateSignal();
    void autofunBuy(auto wallet, auto signal, auto slippageBps);
    std::future<void> handleBuySignal(const std::any& params);
    std::future<void> updateExpectedOutAmount(BuySignalMessage signal);
    void executeBuy(BuySignalMessage signal);

private:
    TokenValidationService validationService_;
    TradeCalculationService calculationService_;
    TradeMemoryService tradeMemoryService_;
};


} // namespace elizaos
