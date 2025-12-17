#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define ISellSignalOutput interface
struct ISellSignalOutput {
    std::string recommended_sell;
    std::string recommend_sell_address;
    std::optional<std::string> reason;
    std::optional<std::string> reason_for_selling;
    std::variant<std::string, double> sell_amount;
    std::optional<std::variant<double, std::string>> slippage;
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

class SellService {
public:
    SellService(IAgentRuntime runtime, WalletService walletService, DataService dataService, AnalyticsService analyticsService, TradeMemoryService tradeMemoryService);
    std::future<void> initialize();
    std::future<void> stop();
    std::future<TradeExecutionResult> generateSignal();
    std::future<void> updateExpectedOutAmount(std::optional<SellSignalMessage> signal);
    void autofunSell(auto signal, auto slippageBps);
    Promise< executeSell(std::optional<SellSignalMessage> signal);

private:
    TokenValidationService validationService_;
    TradeCalculationService calculationService_;
    TradeMemoryService tradeMemoryService_;
};


} // namespace elizaos
