#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/utils/analyzeTrade.h"

double calculateVolatility(array<double> priceHistory)
{
    if (priceHistory->get_length() < 2) return 0;
    auto returns = array<any>();
    for (auto i = 1; i < priceHistory->get_length(); i++)
    {
        returns->push(Math->log(const_(priceHistory)[i] / const_(priceHistory)[i - 1]));
    }
    shared mean = returns->reduce([=](auto a, auto b) mutable
    {
        return a + b;
    }
    ) / returns->get_length();
    auto variance = returns->reduce([=](auto a, auto b) mutable
    {
        return a + Math->pow(b - mean, 2);
    }
    , 0) / returns->get_length();
    return Math->sqrt(variance);
};


double calculateDynamicSlippage(string amount, any quoteData)
{
    try
    {
        auto baseSlippage = 0.005;
        auto priceImpact = Math->abs(parseFloat(OR((quoteData["priceImpactPct"]), (std::string("0"))))) / 100;
        auto amountNum = parseFloat(amount);
        auto decimals = OR((quoteData["inputDecimals"]), (9));
        auto amountInBase = amountNum / Math->pow(10, decimals);
        auto dynamicSlippage = baseSlippage;
        if (priceImpact > 0.01) {
            dynamicSlippage += priceImpact * 0.5;
        }
        if (amountInBase > 1000) {
            dynamicSlippage *= 1.2;
        } else if (amountInBase > 100) {
            dynamicSlippage *= 1.1;
        }
        if (quoteData["marketVolatility"]) {
            dynamicSlippage *= 1 + quoteData["marketVolatility"];
        }
        auto minSlippage = 0.001;
        auto maxSlippage = 0.05;
        return Math->min(Math->max(dynamicSlippage, minSlippage), maxSlippage);
    }
    catch (const any& error)
    {
        logger->warn(std::string("Error calculating dynamic slippage, using default:"), error);
        return 0.01;
    }
};


string tradeAnalysisTemplate = std::string("\
You are a trading assistant focused on managing SOL wallet balances and trade sizes. Your primary goal is to suggest appropriate trade amounts while maintaining safe reserves.\
\
<api_data>\
{{api_data}}\
</api_data>\
\
<market_data>\
{{marketData}}\
</market_data>\
\
Core Rules:\
1. ALWAYS keep minimum 0.002 SOL in wallet for gas fees\
2. Minimum trade size is 5% * {{walletBalance}}\
3. Maximum trade size is 25% * {{walletBalance}} for high volatility tokens\
4. See api_data for token recommendation and market data for technical analysis\
5. suggestedAmount must not exceed walletBalance\
6. Skip trades if wallet balance is too low or market conditions unfavorable\
\
Market Analysis Factors:\
1. Volume Analysis:\
   - 24h volume trend\
   - Volume/Market Cap ratio\
   - Unusual volume spikes\
2. Price Action:\
   - RSI levels\
   - MACD crossovers\
   - Support/Resistance levels\
3. Market Structure:\
   - Liquidity depth\
   - Holder distribution\
   - Recent large transactions\
4. Risk Assessment:\
   - Volatility metrics\
   - Market correlation\
   - Smart money flow\
\
Analyze the following data:\
<wallet_data>\
{{walletBalance}}\
</wallet_data>\
\
Provide an XML response in the following format. Example:\
<trade_analysis>\
  <shouldTrade>true</shouldTrade>\
  <recommendedAction>buy</recommendedAction>\
  <suggestedAmount>0.5</suggestedAmount>\
  <confidence>high</confidence>\
  <reason>Market conditions are favorable.</reason>\
  <riskScore>3</riskScore>\
  <technicalFactors>\
    <trend>bullish</trend>\
    <momentum>80</momentum>\
    <volumeProfile>increasing</volumeProfile>\
    <liquidityScore>7</liquidityScore>\
  </technicalFactors>\
</trade_analysis>\
");

void Main(void)
{
}

MAIN
