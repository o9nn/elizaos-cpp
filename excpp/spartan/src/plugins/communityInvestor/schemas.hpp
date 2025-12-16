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
import type { Transaction } from './types.js';

/**
 * Core schema definitions for community-trader plugin
 * This approach provides runtime validation and better type safety
 */

// Define consistent transaction types
const TransactionType = {
  BUY: 'BUY',
  SELL: 'SELL',
  TRANSFER_IN: 'transfer_in',
  TRANSFER_OUT: 'transfer_out',
} as const;

// Define TokenPerformance schema
const tokenPerformanceSchema = z.object({
  chain: z.string().default('unknown'),
  address: z.string(),
  name: z.string().optional().default(''),
  symbol: z.string(),
  decimals: z.number().default(0),
  metadata: z.record(z.any()).default({}),
  price: z.number().default(0),
  volume: z.number().default(0),
  trades: z.number().default(0),
  liquidity: z.number().default(0),
  holders: z.number().default(0),
  price24hChange: z.number().default(0),
  volume24hChange: z.number().default(0),
  trades24hChange: z.number().default(0),
  holders24hChange: z.number().default(0),
  initialMarketCap: z.number().default(0),
  currentMarketCap: z.number().default(0),
  rugPull: z.boolean().default(false),
  isScam: z.boolean().default(false),
  sustainedGrowth: z.boolean().default(false),
  rapidDump: z.boolean().default(false),
  suspiciousVolume: z.boolean().default(false),
  validationTrust: z.number().default(0),
  createdAt: z.date().default(() => new Date()),
  updatedAt: z.date().default(() => new Date()),
});

// Define Transaction schema - ensure consistency with types.ts
const transactionSchema = z.object({
  id: z.string(),
  positionId: z.string(),
  chain: z.string().default('unknown'),
  tokenAddress: z.string(),
  transactionHash: z.string(),
  // Use lowercase transaction types consistently
  type: z.enum([TransactionType.BUY, TransactionType.SELL]),
  // Use number for amounts consistently
  amount: z.number(),
  price: z.number().optional(),
  isSimulation: z.boolean().default(false),
  // Store timestamps as ISO strings consistently
  timestamp: z.string(),
});

// Define RecommenderMetrics schema
const recommenderMetricsSchema = z.object({
  entityId: z.string(),
  trustScore: z.number(),
  totalRecommendations: z.number(),
  successfulRecs: z.number(),
  avgTokenPerformance: z.number(),
  riskScore: z.number(),
  consistencyScore: z.number(),
  virtualConfidence: z.number(),
  lastActiveDate: z.date(),
  trustDecay: z.number(),
  updatedAt: z
    .date()
    .optional()
    .default(() => new Date()),
});

// Define Position schema
const positionSchema = z.object({
  id: z.string().uuid(),
  chain: z.string(),
  tokenAddress: z.string(),
  walletAddress: z.string(),
  isSimulation: z.boolean(),
  entityId: z.string(),
  recommendationId: z.string(),
  initialPrice: z.string(),
  initialMarketCap: z.string(),
  initialLiquidity: z.string(),
  performanceScore: z.number(),
  rapidDump: z.boolean(),
  openedAt: z.date(),
  closedAt: z.date().optional(),
  updatedAt: z.date(),
  // Store numeric amounts as strings to avoid precision issues
  amount: z.string(),
  entryPrice: z.string(),
  currentPrice: z.string(),
});

// Define TokenRecommendation schema
const tokenRecommendationSchema = z.object({
  id: z.string().uuid(),
  entityId: z.string().uuid(),
  chain: z.string(),
  tokenAddress: z.string(),
  type: z.string(),
  conviction: z.string(),
  initialMarketCap: z.string(),
  initialLiquidity: z.string(),
  initialPrice: z.string(),
  marketCap: z.string(),
  liquidity: z.string(),
  price: z.string(),
  rugPull: z.boolean(),
  isScam: z.boolean(),
  riskScore: z.number(),
  performanceScore: z.number(),
  metadata: z.record(z.any()).default({}),
  status: z.enum(['ACTIVE', 'COMPLETED', 'EXPIRED', 'WITHDRAWN']),
  createdAt: z.date(),
  updatedAt: z.date(),
});

// Types derived from schemas
using TokenPerformance = z.infer<typeof tokenPerformanceSchema>;
using RecommenderMetrics = z.infer<typeof recommenderMetricsSchema>;
using Position = z.infer<typeof positionSchema>;
using TokenRecommendation = z.infer<typeof tokenRecommendationSchema>;

/**
 * Transform functions to convert database objects to schema-validated objects
 */

;

  return tokenPerformanceSchema.parse(input);
}

;

  return transactionSchema.parse(input) as unknown as Transaction;
}

;

  return positionSchema.parse(input);
}

/**
 * Type definition for MessageRecommendation based on the schema recommendationSchema
 */
using MessageRecommendation = z.infer<typeof recommendationSchema>;

/**
 * Schema for extracting trading recommendations from conversational text, capturing the key components of who made the recommendation, what asset was discussed, what action was recommended, and how strongly it was recommended
 */
const recommendationSchema = z
  .object({
    username: z
      .string()
      .describe('The username of the person making the recommendation in the conversation'),

    ticker: z
      .string()
      .optional()
      .nullable()
      .describe(
        "The ticker symbol of the recommended asset (e.g., 'BTC', 'AAPL'). Optional as recommendations may discuss assets without explicit tickers"
      ),

    tokenAddress: z
      .string()
      .optional()
      .nullable()
      .describe(
        'The blockchain contract address of the token if mentioned. This helps disambiguate tokens that might share similar names or symbols'
      ),

    type: z
      .enum(['BUY', 'SELL', 'DONT_BUY', 'DONT_SELL', 'NONE'])
      .describe(
        'The type of trading recommendation being made. This captures both positive recommendations (buy/sell) and explicit warnings against actions'
      ),

    conviction: z
      .enum(['NONE', 'LOW', 'MEDIUM', 'HIGH'])
      .describe(
        'The level of confidence or urgency expressed in the recommendation, helping prioritize stronger signals'
      ),
  })
  .describe(
    'Schema for extracting trading recommendations from conversational text, capturing the key components of who made the recommendation, what asset was discussed, what action was recommended, and how strongly it was recommended'
  );

,
      status: dbRec.status || 'ACTIVE',
      createdAt: new Date(dbRec.createdAt || dbRec.created_at || Date.now()),
      updatedAt: new Date(dbRec.updatedAt || dbRec.updated_at || Date.now()),
    });
  } catch (error) {
    console.error('Error transforming token recommendation:', error);
    return null;
  }
}

} // namespace elizaos
