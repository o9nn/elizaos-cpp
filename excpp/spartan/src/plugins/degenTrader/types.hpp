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

;

// Token Security Types
struct TokenSecurityData {
    std::string ownerBalance;
    std::string creatorBalance;
    double ownerPercentage;
    double creatorPercentage;
    std::string top10HolderBalance;
    double top10HolderPercent;
};


// Token Trading Types
struct TokenTradeData {
    double price;
    double priceChange24h;
    double volume24h;
    std::string volume24hUsd;
    double uniqueWallets24h;
    double uniqueWallets24hChange;
};


struct DexScreenerPair {
    double priceUsd;
    { h24: number } volume;
    double marketCap;
    { usd: number; base: number } liquidity;
    { h24: number } priceChange;
    { h24: { buys: number; sells: number } } txns;
};


struct ProcessedTokenData {
    TokenSecurityData security;
    TokenTradeData tradeData;
    { pairs: DexScreenerPair[] } dexScreenerData;
    std::string holderDistributionTrend;
    std::vector<std::any> highValueHolders;
    bool recentTrades;
    double highSupplyHoldersCount;
};


// Market and Position Types
using MarketData = {
  priceChange24h: number;
  volume24h: number;
  liquidity: {
    usd: number;
  };
};

using Position = {
  token: string;
  tokenAddress: string;
  entryPrice: number;
  amount: number;
  timestamp: number;
  sold?: boolean;
  exitPrice?: number;
  exitTimestamp?: number;
  initialMetrics: {
    trustScore: number;
    volume24h: number;
    liquidity: { usd: number };
    riskLevel: 'LOW' | 'MEDIUM' | 'HIGH';
  };
  highestPrice?: number;
  partialTakeProfit?: boolean;
};

// Analysis Types
using TokenAnalysis = {
  security: {
    ownerBalance: string;
    creatorBalance: string;
    ownerPercentage: number;
    top10HolderPercent: number;
  };
  trading: {
    price: number;
    priceChange24h: number;
    volume24h: number;
    uniqueWallets24h: number;
    walletChanges: {
      unique_wallet_30m_change_percent: number;
      unique_wallet_1h_change_percent: number;
      unique_wallet_24h_change_percent: number;
    };
  };
  market: {
    liquidity: number;
    marketCap: number;
    fdv: number;
  };
};

struct TokenAnalysisState {
    double lastAnalyzedIndex;
    std::unordered_set<std::string> analyzedTokens;
};


// Signal Types
struct BuySignalMessage {
    std::string positionId;
    std::string tokenAddress;
    std::string tradeAmount;
    std::string expectedOutAmount;
    std::string entityId;
};


struct SellSignalMessage {
    std::string positionId;
    std::string tokenAddress;
    std::optional<std::string> pairId;
    std::string amount;
    std::optional<std::string> currentBalance;
    std::optional<std::string> sellRecommenderId;
    std::optional<std::string> walletAddress;
    std::optional<bool> isSimulation;
    std::optional<std::string> reason;
    std::optional<std::string> entityId;
    std::optional<double> slippage;
};


struct QuoteParams {
    std::string inputMint;
    std::string outputMint;
    std::string amount;
    std::string walletAddress;
    double slippageBps;
};


struct StartProcessParams {
    std::string id;
    std::string tokenAddress;
    std::string balance;
    bool isSimulation;
    std::string initialMarketCap;
    std::string entityId;
    std::optional<std::string> walletAddress;
    std::optional<std::string> txHash;
};


struct AddTransactionParams {
    std::string id;
    std::string address;
    std::string amount;
    std::string walletAddress;
    bool isSimulation;
    double marketCap;
    std::string entityId;
    std::string txHash;
};


struct PriceSignalMessage {
    std::string initialPrice;
    std::string currentPrice;
    double priceChange;
    std::string tokenAddress;
};


interface StartDegenProcessParams extends StartProcessParams {
  initialPrice: string;
}

struct ITradeService {
    { dataService;
    (tokens: string[]) => Promise<any> getTokensMarketData;
};


const ServiceTypes = {
  DEGEN_TRADING: 'degen_trader',
} as const;

} // namespace elizaos
