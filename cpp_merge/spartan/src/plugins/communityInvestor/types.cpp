#include "types.hpp"

void Main(void)
{
    string_empty + infer + string_empty;
    Start;
    {
        {
            $;
            {
                infer;
                Var;
            }
        }
    }
    $;
    {
        infer;
        Rest;
    }
    std:("\
  ? Var | ExtractVariables<Rest>\
  : never;\
\
/**\
 * Represents a type that defines template variables for a given string type.\
 *\
 * @template T - The string type for which template variables are defined.\
 * @typedef TemplateVariables\
 * @type {Pretty<{ [K in ExtractVariables<T>]; }>}\
 */\
type TemplateVariables<T extends string> = Pretty<{\
  [K in ExtractVariables<T>];\
}>;\
\
/**\
 * Represents a value that can be stored in a SQLite database, which can be a string, number, or null.\
 */\
type SQLiteValue = string | number | null;\
\
/**\
 * Type utility for converting TypeScript types to SQLite column types.\
 *\
 * @template T - The TypeScript type to convert.\
 * @param {T} - The value to convert.\
 * @returns {ToSQLiteType<T>} - The SQLite column type equivalent of the input type.\
 */\
type ToSQLiteType<T> = T extends boolean\
  ? number\
  : T extends Date\
    ? string\
    : T extends bigint\
      ? string\
      : T extends Array<any>\
        ? string\
        : T extends object\
          ? string\
          : T extends SQLiteValue\
            ? T\
            : never;\
\
/**\
 * Converts a generic record type to a SQLite record type, where each property value is converted to a SQLite type.\
 *\
 * @template T - The generic record type to be converted to a SQLite record type.\
 * @typedef ToSQLiteRecord\
 * @type {object}\
 */\
type ToSQLiteRecord<T extends Record<string, any>> = {\
  [K in keyof T]: ToSQLiteType<T[K]>;\
};\
\
/**\
 * Represents a type which is used to define a single row in the database table for RecommenderMetrics.\
 */\
type RecommenderMetricsRow = ToSQLiteRecord<RecommenderMetrics>;\
/**\
 * Defines an alias for converting a TokenPerformance object into a SQLite record format.\
 */\
type TokenPerformanceRow = ToSQLiteRecord<TokenPerformance>;\
/**\
 * Represents a single row of data in a SQLite database table, corresponding to the Position model.\
 */\
type PositionRow = ToSQLiteRecord<Position>;\
/**\
 * A type alias representing a row in the Transaction table,\
 * serialized as a SQLite record.\
 */\
type TransactionRow = ToSQLiteRecord<Transaction>;\
\
/**\
 * Interface representing the metrics of a recommender.\
 * @typedef {{\
 *    entityId: UUID,\
 *    platform,\
 *    totalRecommendations,\
 *    successfulRecs,\
 *    failedTrades,\
 *    totalProfit,\
 *    avgTokenPerformance,\
 *    consistencyScore,\
 *    trustScore,\
 *    lastUpdated: Date,\
 *    createdAt: Date\
 * }} RecommenderMetrics\
 */\
interface RecommenderMetrics {\
  entityId: UUID;\
  platform;\
  totalRecommendations;\
  successfulRecs;\
  failedTrades;\
  totalProfit;\
  avgTokenPerformance;\
  consistencyScore;\
  trustScore;\
  lastUpdated: Date;\
  createdAt: Date;\
}\
\
/**\
 * Interface representing the history of recommender metrics for a specific entity.\
 * @typedef {Object} RecommenderMetricsHistory\
 * @property {UUID} entityId - The ID of the entity for which the metrics are recorded.\
 * @property {RecommenderMetrics} metrics - The metrics related to the entity.\
 * @property {Date} timestamp - The timestamp when the metrics were recorded.\
 */\
interface RecommenderMetricsHistory {\
  entityId: UUID;\
  metrics: RecommenderMetrics;\
  timestamp: Date;\
}\
\
/**\
 * Interface representing performance data for a token.\
 * @typedef {Object} TokenPerformance\
 * @property {string} [chain] - The blockchain network the token belongs to.\
 * @property {string} [address] - The address of the token.\
 * @property {string} [name] - The name of the token.\
 * @property {string} [symbol] - The symbol of the token.\
 * @property {number} [decimals] - The number of decimal places for the token.\
 * @property {Object.<string, any>} [metadata] - Additional metadata for the token.\
 * @property {number} [price] - The current price of the token.\
 * @property {number} [price24hChange] - The percentage change in price over the last 24 hours.\
 * @property {number} [volume] - The trading volume of the token.\
 * @property {number} [volume24hChange] - The percentage change in trading volume over the last 24 hours.\
 * @property {number} [trades] - The number of trades for the token.\
 * @property {number} [trades24hChange] - The percentage change in number of trades over the last 24 hours.\
 * @property {number} [liquidity] - The liquidity of the token.\
 * @property {number} [holders] - The number of holders of the token.\
 * @property {number} [holders24hChange] - The percentage change in number of holders over the last 24 hours.\
 * @property {number} [initialMarketCap] - The initial market capitalization of the token.\
 * @property {number} [currentMarketCap] - The current market capitalization of the token.\
 * @property {boolean} [rugPull] - Indicates if the token is associated with a rug pull.\
 * @property {boolean} [isScam] - Indicates if the token is considered a scam.\
 * @property {boolean} [sustainedGrowth] - Indicates if the token has shown sustained growth.\
 * @property {boolean} [rapidDump] - Indicates if the token has experienced a rapid dump in price.\
 * @property {boolean} [suspiciousVolume] - Indicates if the token has suspicious trading volume.\
 * @property {number} [validationTrust] - The level of trust in the token's validation.\
 * @property {Date} [createdAt] - The date and time when the token performance data was created.\
 * @property {Date} [updatedAt] - The date and time when the token performance data was last updated.\
 */\
interface TokenPerformance {\
  chain?;\
  address?;\
  name?;\
  symbol?;\
  decimals?;\
  metadata?: Record<string, any>;\
  price?;\
  price24hChange?;\
  volume?;\
  volume24hChange?;\
  trades?;\
  trades24hChange?;\
  liquidity?;\
  holders?;\
  holders24hChange?;\
  initialMarketCap?;\
  currentMarketCap?;\
  rugPull?;\
  isScam?;\
  sustainedGrowth?;\
  rapidDump?;\
  suspiciousVolume?;\
  validationTrust?;\
  createdAt?: Date;\
  updatedAt?: Date;\
}\
\
/**\
 * Conviction levels for recommendations\
 * IMPORTANT: Must match the enum in config.ts\
 */\
/**\
 * Enumeration representing levels of conviction.\
 * @readonly\
 * @enum {string}\
 * @property {string} NONE - No conviction.\
 * @property {string} LOW - Low level of conviction.\
 * @property {string} MEDIUM - Medium level of conviction.\
 * @property {string} HIGH - High level of conviction.\
 * @property {string} VERY_HIGH - Very high level of conviction.\
 */\
enum Conviction {\
  NONE = 'NONE',\
  LOW = 'LOW',\
  MEDIUM = 'MEDIUM',\
  HIGH = 'HIGH',\
  VERY_HIGH = 'VERY_HIGH',\
}\
\
/**\
 * Recommendation types\
 * IMPORTANT: Must match the enum in config.ts\
 */\
enum RecommendationType {\
  BUY = 'BUY',\
  DONT_BUY = 'DONT_BUY',\
  SELL = 'SELL',\
  DONT_SELL = 'DONT_SELL',\
  NONE = 'NONE',\
  HOLD = 'HOLD',\
}\
\
type TokenRecommendation = {\
  id: UUID;\
  entityId: UUID;\
  chain;\
  tokenAddress;\
  conviction: Conviction;\
  type: RecommendationType;\
  initialMarketCap;\
  initialLiquidity;\
  initialPrice;\
  marketCap;\
  liquidity;\
  price;\
  rugPull;\
  isScam;\
  riskScore;\
  performanceScore;\
  metadata: Record<string, any>;\
  status: 'ACTIVE' | 'COMPLETED' | 'EXPIRED' | 'WITHDRAWN';\
  createdAt: Date;\
  updatedAt: Date;\
};\
\
interface Position {\
  id: UUID;\
  entityId: UUID;\
  tokenAddress;\
  chain;\
  walletAddress;\
  balance;\
  status: 'OPEN' | 'CLOSED';\
  createdAt: Date;\
  closedAt?: Date;\
  isSimulation;\
  amount;\
  initialPrice;\
  currentPrice?;\
  recommendationId: UUID;\
}\
\
type PositionWithBalance = Position & {\
  balance: bigint;\
};\
\
/**\
 * Unified transaction type enums to ensure consistency\
 * IMPORTANT: Must match the enum in config.ts\
 */\
enum TransactionType {\
  BUY = 'BUY',\
  SELL = 'SELL',\
  TRANSFER_IN = 'transfer_in',\
  TRANSFER_OUT = 'transfer_out',\
}\
\
/**\
 * Complete transaction interface with all possible fields\
 */\
interface Transaction {\
  id: UUID;\
  positionId: UUID;\
  tokenAddress;\
  type: TransactionType;\
  amount;\
  valueUsd?;\
  marketCap?;\
  liquidity?;\
  price;\
  isSimulation;\
  timestamp: Date;\
  chain?;\
  transactionHash?;\
}\
\
type SellDetails = {\
  price;\
  timestamp;\
  amount: bigint;\
  receivedSol: bigint;\
  valueUsd;\
  profitUsd;\
  profitPercent;\
  marketCap;\
  marketCapChange;\
  liquidity;\
  liquidityChange;\
  rapidDump;\
  entityId;\
};\
\
type BuyData = {\
  positionId;\
  chain;\
  tokenAddress;\
  walletAddress;\
  entityID: UUID;\
  recommendationId;\
  solAmount: bigint;\
  buyAmount: bigint;\
  timestamp: Date;\
  initialTokenPriceUsd;\
  isSimulation;\
  txHash;\
};\
\
type SellData = {\
  positionId;\
  chain;\
  tokenAddress;\
  walletAddress;\
  entityID: UUID;\
  solAmount: bigint;\
  sellAmount: bigint;\
  timestamp: Date;\
  isSimulation;\
  txHash;\
};\
\
type RecommenderAnalytics = {\
  entityId;\
  trustScore;\
  riskScore;\
  consistencyScore;\
  recommenderMetrics: RecommenderMetrics;\
};\
\
type TokenRecommendationSummary = {\
  chain;\
  tokenAddress;\
  averageTrustScore;\
  averageRiskScore;\
  averageConsistencyScore;\
  recommenders: RecommenderAnalytics[];\
};\
\
type TransactionData = {\
  chain;\
  tokenAddress;\
  pairId;\
  amount;\
  currentBalance;\
  sellRecommenderId;\
  walletAddress;\
  transaction | null;\
  isSimulation;\
};\
\
type QuoteResult<Data = any> = {\
  amountOut: bigint;\
  data?: Data;\
};\
\
type SwapInResult<Data = any> = {\
  txHash;\
  amountOut: bigint;\
  timestamp: Date;\
  data?: Data;\
};\
\
type QuoteInParams = {\
  inputToken;\
  outputToken;\
  amountIn: bigint;\
  slippageBps?;\
};\
\
type SwapInParams<SwapData = any> = {\
  inputToken;\
  outputToken;\
  amountIn: bigint;\
  minAmountOut: bigint;\
  isSimulation;\
  data?: SwapData;\
};\
\
interface TrustWalletProvider<\
  QuoteData = any,\
  TQuoteResult extends QuoteResult<QuoteData> = QuoteResult<QuoteData>,\
  SwapResultData = any,\
  TSwapResult extends SwapInResult<SwapResultData> = SwapInResult<SwapResultData>,\
> {\
  getCurrencyAddress();\
  getAddress();\
  getQuoteIn(props: QuoteInParams): Promise<TQuoteResult>;\
  swapIn(props: SwapInParams<QuoteData>): Promise<TSwapResult>;\
\
  executeSwap<SwapData = any, SwapResultData = any>(params: {\
    inputToken;\
    outputToken;\
    swapData: SwapData;\
  }): Promise<SwapInResult<SwapResultData>>;\
\
  getTokenFromWallet(tokenSymbol): Promise<string | null>;\
  getAccountBalance(): Promise<bigint>;\
}\
\
type TokenMetadata = {\
  chain;\
  address;\
  name;\
  symbol;\
  decimals;\
  metadata: Record<string, any>;\
};\
\
type TokenMarketData = {\
  price;\
  priceUsd;\
  price24hChange;\
\
  marketCap;\
\
  uniqueWallet24h;\
  uniqueWallet24hChange;\
\
  volume24h;\
  volume24hChange;\
\
  trades;\
  trades24hChange;\
\
  liquidityUsd;\
\
  holders;\
};\
\
// Message recommendation extracted from text\
interface MessageRecommendation {\
  tokenMentioned;\
  isTicker;\
  sentiment: 'positive' | 'negative' | 'neutral';\
  conviction: 'NONE' | 'LOW' | 'MEDIUM' | 'HIGH';\
  quote;\
}\
\
interface RecommendationMemory extends Memory {\
  content: Content & {\
    recommendation: MessageRecommendation & {\
      confirmed?;\
    };\
  };\
}\
\
// TODO: Consolidate this into "Entity" with metadata\
type Account = {\
  id: UUID;\
  name;\
  username;\
  email;\
  avatarUrl;\
  telegramId;\
  discordId;\
};\
\
type TokenTradeData = {\
  address;\
  holder;\
  market;\
  last_trade_unix_time;\
  last_trade_human_time;\
  price;\
  history_30m_price;\
  price_change_30m_percent;\
  history_1h_price;\
  price_change_1h_percent;\
  history_2h_price;\
  price_change_2h_percent;\
  history_4h_price;\
  price_change_4h_percent;\
  history_6h_price;\
  price_change_6h_percent;\
  history_8h_price;\
  price_change_8h_percent;\
  history_12h_price;\
  price_change_12h_percent;\
  history_24h_price;\
  price_change_24h_percent;\
  unique_wallet_30m;\
  unique_wallet_history_30m;\
  unique_wallet_30m_change_percent;\
  unique_wallet_1h;\
  unique_wallet_history_1h;\
  unique_wallet_1h_change_percent;\
  unique_wallet_2h;\
  unique_wallet_history_2h;\
  unique_wallet_2h_change_percent;\
  unique_wallet_4h;\
  unique_wallet_history_4h;\
  unique_wallet_4h_change_percent;\
  unique_wallet_8h;\
  unique_wallet_history_8h | null;\
  unique_wallet_8h_change_percent | null;\
  unique_wallet_24h;\
  unique_wallet_history_24h | null;\
  unique_wallet_24h_change_percent | null;\
  trade_30m;\
  trade_history_30m;\
  trade_30m_change_percent;\
  sell_30m;\
  sell_history_30m;\
  sell_30m_change_percent;\
  buy_30m;\
  buy_history_30m;\
  buy_30m_change_percent;\
  volume_30m;\
  volume_30m_usd;\
  volume_history_30m;\
  volume_history_30m_usd;\
  volume_30m_change_percent;\
  volume_buy_30m;\
  volume_buy_30m_usd;\
  volume_buy_history_30m;\
  volume_buy_history_30m_usd;\
  volume_buy_30m_change_percent;\
  volume_sell_30m;\
  volume_sell_30m_usd;\
  volume_sell_history_30m;\
  volume_sell_history_30m_usd;\
  volume_sell_30m_change_percent;\
  trade_1h;\
  trade_history_1h;\
  trade_1h_change_percent;\
  sell_1h;\
  sell_history_1h;\
  sell_1h_change_percent;\
  buy_1h;\
  buy_history_1h;\
  buy_1h_change_percent;\
  volume_1h;\
  volume_1h_usd;\
  volume_history_1h;\
  volume_history_1h_usd;\
  volume_1h_change_percent;\
  volume_buy_1h;\
  volume_buy_1h_usd;\
  volume_buy_history_1h;\
  volume_buy_history_1h_usd;\
  volume_buy_1h_change_percent;\
  volume_sell_1h;\
  volume_sell_1h_usd;\
  volume_sell_history_1h;\
  volume_sell_history_1h_usd;\
  volume_sell_1h_change_percent;\
  trade_2h;\
  trade_history_2h;\
  trade_2h_change_percent;\
  sell_2h;\
  sell_history_2h;\
  sell_2h_change_percent;\
  buy_2h;\
  buy_history_2h;\
  buy_2h_change_percent;\
  volume_2h;\
  volume_2h_usd;\
  volume_history_2h;\
  volume_history_2h_usd;\
  volume_2h_change_percent;\
  volume_buy_2h;\
  volume_buy_2h_usd;\
  volume_buy_history_2h;\
  volume_buy_history_2h_usd;\
  volume_buy_2h_change_percent;\
  volume_sell_2h;\
  volume_sell_2h_usd;\
  volume_sell_history_2h;\
  volume_sell_history_2h_usd;\
  volume_sell_2h_change_percent;\
  trade_4h;\
  trade_history_4h;\
  trade_4h_change_percent;\
  sell_4h;\
  sell_history_4h;\
  sell_4h_change_percent;\
  buy_4h;\
  buy_history_4h;\
  buy_4h_change_percent;\
  volume_4h;\
  volume_4h_usd;\
  volume_history_4h;\
  volume_history_4h_usd;\
  volume_4h_change_percent;\
  volume_buy_4h;\
  volume_buy_4h_usd;\
  volume_buy_history_4h;\
  volume_buy_history_4h_usd;\
  volume_buy_4h_change_percent;\
  volume_sell_4h;\
  volume_sell_4h_usd;\
  volume_sell_history_4h;\
  volume_sell_history_4h_usd;\
  volume_sell_4h_change_percent;\
  trade_8h;\
  trade_history_8h | null;\
  trade_8h_change_percent | null;\
  sell_8h;\
  sell_history_8h | null;\
  sell_8h_change_percent | null;\
  buy_8h;\
  buy_history_8h | null;\
  buy_8h_change_percent | null;\
  volume_8h;\
  volume_8h_usd;\
  volume_history_8h;\
  volume_history_8h_usd;\
  volume_8h_change_percent | null;\
  volume_buy_8h;\
  volume_buy_8h_usd;\
  volume_buy_history_8h;\
  volume_buy_history_8h_usd;\
  volume_buy_8h_change_percent | null;\
  volume_sell_8h;\
  volume_sell_8h_usd;\
  volume_sell_history_8h;\
  volume_sell_history_8h_usd;\
  volume_sell_8h_change_percent | null;\
  trade_24h;\
  trade_history_24h;\
  trade_24h_change_percent | null;\
  sell_24h;\
  sell_history_24h;\
  sell_24h_change_percent | null;\
  buy_24h;\
  buy_history_24h;\
  buy_24h_change_percent | null;\
  volume_24h;\
  volume_24h_usd;\
  volume_history_24h;\
  volume_history_24h_usd;\
  volume_24h_change_percent | null;\
  volume_buy_24h;\
  volume_buy_24h_usd;\
  volume_buy_history_24h;\
  volume_buy_history_24h_usd;\
  volume_buy_24h_change_percent | null;\
  volume_sell_24h;\
  volume_sell_24h_usd;\
  volume_sell_history_24h;\
  volume_sell_history_24h_usd;\
  volume_sell_24h_change_percent | null;\
};\
\
type HolderData = {\
  address;\
  balance;\
};\
\
type TokenSecurityData = {\
  ownerBalance;\
  creatorBalance;\
  ownerPercentage;\
  creatorPercentage;\
  top10HolderBalance;\
  top10HolderPercent;\
};\
\
type ProcessedTokenData = {\
  token: TokenOverview;\
  security: TokenSecurityData;\
  tradeData: TokenTradeData;\
  holderDistributionTrend; // 'increasing' | 'decreasing' | 'stable'\
  highValueHolders: {\
    holderAddress;\
    balanceUsd;\
  }[];\
  recentTrades;\
  highSupplyHoldersCount;\
  dexScreenerData: DexScreenerData;\
\
  isDexScreenerListed;\
  isDexScreenerPaid;\
};\
\
type DexScreenerPair = {\
  chainId;\
  dexId;\
  url;\
  pairAddress;\
  baseToken: {\
    address;\
    name;\
    symbol;\
  };\
  quoteToken: {\
    address;\
    name;\
    symbol;\
  };\
  priceNative;\
  priceUsd;\
  txns: {\
    m5: { buys; sells };\
    h1: { buys; sells };\
    h6: { buys; sells };\
    h24: { buys; sells };\
  };\
  volume: {\
    h24;\
    h6;\
    h1;\
    m5;\
  };\
  priceChange: {\
    m5;\
    h1;\
    h6;\
    h24;\
  };\
  liquidity?: {\
    usd;\
    base;\
    quote;\
  };\
  fdv;\
  marketCap;\
  pairCreatedAt;\
  info: {\
    imageUrl;\
    websites: { label; url }[];\
    socials: { type; url }[];\
  };\
  boosts: {\
    active;\
  };\
};\
\
type DexScreenerData = {\
  schemaVersion;\
  pairs: DexScreenerPair[];\
};\
\
type Prices = {\
  solana: { usd };\
  bitcoin: { usd };\
  ethereum: { usd };\
};\
\
type CalculatedBuyAmounts = {\
  none: 0;\
  low;\
  medium;\
  high;\
};\
\
type WalletPortfolioItem = {\
  name;\
  address;\
  symbol;\
  decimals;\
  balance;\
  uiAmount;\
  priceUsd;\
  valueUsd;\
  valueSol?;\
};\
\
type WalletPortfolio = {\
  totalUsd;\
  totalSol?;\
  items: WalletPortfolioItem[];\
};\
\
type TokenOverview = {\
  address;\
  name;\
  symbol;\
  decimals?;\
  logoURI?;\
};\
\
interface BuySignalMessage {\
  positionId?;\
  tokenAddress;\
  chain;\
  walletAddress;\
  isSimulation;\
  entityId;\
  recommendationId;\
  price;\
  marketCap;\
  liquidity;\
  amount;\
  type: RecommendationType;\
  conviction: Conviction;\
}\
\
interface Trade {\
  id;\
  positionId;\
  type: TransactionType.BUY | TransactionType.SELL;\
  amount: bigint;\
  price: bigint;\
  timestamp: Date;\
  txHash;\
}\
\
interface TradePerformance {\
  token_address;\
  recommender_id;\
  buy_price;\
  sell_price;\
  buy_timeStamp;\
  sell_timeStamp;\
  buy_amount;\
  sell_amount;\
  buy_sol;\
  received_sol;\
  buy_value_usd;\
  sell_value_usd;\
  profit_usd;\
  profit_percent;\
  buy_market_cap;\
  sell_market_cap;\
  market_cap_change;\
  buy_liquidity;\
  sell_liquidity;\
  liquidity_change;\
  last_updated;\
  rapidDump;\
}\
\
/**\
 * Represents the metrics of a trade including total bought quantity, total bought value, total sold quantity,\
 * total sold value, total transfer in quantity, total transfer out quantity, average entry price, average exit price,\
 * realized profit and loss, realized profit and loss percentage, volume in USD, first trade time, and last trade time.\
 * @typedef {Object} TradeMetrics\
 * @property {number} totalBought - The total quantity bought\
 * @property {number} totalBoughtValue - The total value of items bought\
 * @property {number} totalSold - The total quantity sold\
 * @property {number} totalSoldValue - The total value of items sold\
 * @property {number} totalTransferIn - The total quantity transferred in\
 * @property {number} totalTransferOut - The total quantity transferred out\
 * @property {number} averageEntryPrice - The average price at which items were bought\
 * @property {number} averageExitPrice - The average price at which items were sold\
 * @property {number} realizedPnL - The realized profit and loss\
 * @property {number} realizedPnLPercent - The realized profit and loss percentage\
 * @property {number} volumeUsd - The volume in USD\
 * @property {Date} firstTradeTime - The timestamp of the first trade\
 * @property {Date} lastTradeTime - The timestamp of the last trade\
 */\
type TradeMetrics = {\
  totalBought;\
  totalBoughtValue;\
  totalSold;\
  totalSoldValue;\
  totalTransferIn;\
  totalTransferOut;\
  averageEntryPrice;\
  averageExitPrice;\
  realizedPnL;\
  realizedPnLPercent;\
  volumeUsd;\
  firstTradeTime: Date;\
  lastTradeTime: Date;\
};\
\
/**\
 * Type for position performance statistics.\
 * Includes information about the position such as token, current value, initial value, profit/loss, profit/loss percentage,\
 * price change, price change percentage, normalized balance, trade metrics, unrealized profit/loss, unrealized profit/loss percentage,\
 * total profit/loss, and total profit/loss percentage.\
 */\
type PositionPerformance = Pretty<\
  PositionWithBalance & {\
    token: TokenPerformance;\
    currentValue;\
    initialValue;\
    profitLoss;\
    profitLossPercentage;\
    priceChange;\
    priceChangePercentage;\
    normalizedBalance;\
    trades: TradeMetrics;\
    unrealizedPnL;\
    unrealizedPnLPercent;\
    totalPnL;\
    totalPnLPercent;\
  }\
>;\
\
// ServiceType Enum to identify the service within the runtime\
enum ServiceType {\
  COMMUNITY_INVESTOR = 'community-investor',\
}\
\
// Supported cryptocurrency chains\
enum SupportedChain {\
  SOLANA = 'SOLANA',\
  ETHEREUM = 'ETHEREUM',\
  BASE = 'BASE',\
  UNKNOWN = 'UNKNOWN', // For cases where chain can't be determined\
}\
\
// Metrics calculated after observing a recommendation's market performance\
interface RecommendationMetric {\
  potentialProfitPercent?; // e.g., based on ATH after recommendation or price after X days for BUYs\
  avoidedLossPercent?; // For SELL/criticism, based on price drop avoided\
  isScamOrRug?; // Flagged based on heuristics\
  evaluationTimestamp; // When this metric was last calculated\
  notes?; // e.g., "Hit ATH 3 days later", "Rug pulled", "Low liquidity spike"\
}\
\
// Represents a single recommendation or criticism made by a user\
interface Recommendation {\
  id: UUID; // Unique ID for this recommendation instance\
  userId: UUID; // Entity ID of the recommender\
  messageId: UUID; // Original message ID that sparked this recommendation\
  timestamp; // When the recommendation was made (from original message)\
  tokenTicker?; // e.g., "SOL", "BTC" (if identified as a ticker)\
  tokenAddress; // e.g., "So11111111111111111111111111111111111111112"\
  chain: SupportedChain; // The blockchain the token is on\
  recommendationType: 'BUY' | 'SELL'; // 'SELL' for criticisms\
  conviction: 'NONE' | 'LOW' | 'MEDIUM' | 'HIGH'; // Sender's conviction level\
  rawMessageQuote; // The exact text snippet that is the recommendation/criticism\
  priceAtRecommendation?; // Price of the token when the recommendation was made\
  metrics?: RecommendationMetric; // Performance metrics, calculated later by a task\
  processedForTradeDecision?; // Has the PROCESS_TRADE_DECISION task run for this?\
}\
\
// Data structure for the component stored on an Entity\
interface UserTrustProfile {\
  version; // Schema version, e.g., "1.0.0"\
  userId: UUID; // Entity ID this profile belongs to\
  trustScore; // Weighted average score from -100 to 100\
  lastTrustScoreCalculationTimestamp; // When trustScore was last calculated\
  lastTradeDecisionMadeTimestamp?; // For the 12-hour cooldown for *acting* on this user's recs\
  recommendations: Recommendation[]; // Array of recommendations made by this user\
}\
\
// Type alias for the data field within the ElizaOS Component\
type TrustMarketplaceComponentData = UserTrustProfile;\
\
// Constant for the component type name used in runtime.getComponent/createComponent\
const TRUST_MARKETPLACE_COMPONENT_TYPE = 'communityInvestorProfile';\
\
// Payload for the PROCESS_TRADE_DECISION task\
interface TradeDecisionInput {\
  recommendationId: UUID;\
  userId: UUID;\
}\
\
// Structure for data fetched from external token APIs (e.g., Birdeye, DexScreener)\
interface TokenAPIData {\
  name?;\
  symbol?;\
  currentPrice?;\
  ath?; // All-Time High\
  atl?; // All-Time Low\
  priceAtRecommendation?; // Price snapshot when the recommendation was made (if fetched at that time)\
  priceHistory?: Array<{ timestamp; price }>; // For historical analysis\
  liquidity?;\
  marketCap?;\
  isKnownScam?; // From external data sources if available\
}\
\
// Data structure for frontend leaderboard entries\
interface LeaderboardEntry {\
  userId: UUID;\
  username?; // Display name for the user\
  trustScore;\
  rank?; // Calculated dynamically\
  recommendations: Recommendation[]; // Full recommendation history for drill-down\
}\
\
// Interface defining the methods our CommunityInvestorService will provide\
// This helps ensure the class implements all necessary functions.\
interface ICommunityInvestorService {\
  initialize(runtime: IAgentRuntime): Promise<void>;\
  resolveTicker(\
    ticker,\
    chain: SupportedChain,\
    contextMessages: Memory[]\
  ): Promise<{ address; chain: SupportedChain; ticker? } | null>;\
  getTokenAPIData(address, chain: SupportedChain): Promise<TokenAPIData | null>;\
  isLikelyScamOrRug(tokenData: TokenAPIData, recommendationTimestamp): Promise<boolean>;\
  evaluateRecommendationPerformance(\
    recommendation: Recommendation,\
    tokenData: TokenAPIData\
  ): Promise<RecommendationMetric>;\
  calculateUserTrustScore(userId: UUID, runtime: IAgentRuntime): Promise<void>;\
  getRecencyWeight(recommendationTimestamp);\
  getConvictionWeight(conviction: Recommendation['conviction']);\
  getLeaderboardData(runtime: IAgentRuntime): Promise<LeaderboardEntry[]>;\
  // ensureTaskWorkersRegistered(runtime: IAgentRuntime); // Task registration is internal to constructor\
}\
\
// Adding MessageReceivedHandlerParams\
interface MessageReceivedHandlerParams {\
  runtime: IAgentRuntime;\
  message: Memory;\
  callback: [&](\
    response | Record<string, any>,\
    metadata?: Record<string, any>\
  ) { return Promise<void>; };\
  onComplete?: [&]() { return void; };\
}\
");
}

MAIN
