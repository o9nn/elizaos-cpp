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



struct IBuySignalOutput {
    std::string recommended_buy;
    std::string recommend_buy_address;
    std::string reason;
    string | number buy_amount;
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

class BuyService extends BaseTradeService {
  private validationService: TokenValidationService;
  private calculationService: TradeCalculationService;
  private tradeMemoryService: TradeMemoryService;

  constructor(
    runtime: IAgentRuntime,
    walletService: WalletService,
    dataService: DataService,
    analyticsService: AnalyticsService,
    tradeMemoryService: TradeMemoryService
  ) {
    super(runtime, walletService, dataService, analyticsService);
    this.validationService = new TokenValidationService(
      runtime,
      walletService,
      dataService,
      analyticsService
    );
    this.calculationService = new TradeCalculationService(
      runtime,
      walletService,
      dataService,
      analyticsService
    );
    this.tradeMemoryService = tradeMemoryService;
  }

    // Cleanup if needed

    // get data from plugin-auto
    //const tradeService = runtime.getService(ServiceTypes.AUTOFUN_TRADING)
    // or make call ourself

    /*
*/

    // inject into prompt

    // get balance from plugin-solana

    // run llm call
    // Retry if missing required fields
    // recommended_buy, recommend_buy_address, reason, buy_amount
      // could use OBJECT_LARGE but this expects a string return type rn
      // not sure where OBJECT_LARGE does it's parsing...

    //const walletBalance = await this.walletService.getBalance();

    // what type of token is this, prebonded or post-bonded?

    //console.log('buy params', params);

    //console.log('token', token)

    // for anchor

    // Use the imported IDL for typing, cast to any to bypass potential strict type mismatch

    // is this right?

    //const walletKeypair = getWalletKeypair(runtime);

    // Get fresh blockhash with processed commitment for speed

    // Send transaction

    //signature = await wallet.sendTransaction(versionedTx, connection);


} // namespace elizaos
