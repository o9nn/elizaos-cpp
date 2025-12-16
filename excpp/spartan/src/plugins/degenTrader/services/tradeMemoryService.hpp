#include "analyticsService.hpp"
#include "base/BaseTradeService.hpp"
#include "dataService.hpp"
#include "elizaos/core.hpp"
#include "walletService.hpp"
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
;
;
;
;
;

struct TradeMemory {
    UUID id;
    std::string tokenAddress;
    std::string chain;
    'BUY' | 'SELL' type;
    std::string amount;
    string; // in USD price;
    Date timestamp;
    std::optional<std::string> txHash;
    std::optional<{> metadata;
    std::optional<double> slippage;
    std::optional<std::string> expectedAmount;
    std::optional<std::string> receivedAmount;
    std::optional<std::string> valueUsd;
};


class TradeMemoryService extends BaseTradeService {
  constructor(
    runtime: IAgentRuntime,
    walletService: WalletService,
    dataService: DataService,
    analyticsService: AnalyticsService
  ) {
    super(runtime, walletService, dataService, analyticsService);
  }

  async initialize(): Promise<void> {
    logger.info('Initializing trade memory service');
  }

  async storeTrade(trade: TradeMemory): Promise<void> {
    try {
      const memoryContent = `${trade.type} trade for ${trade.tokenAddress} on ${trade.chain} at ${trade.timestamp.toISOString()}. Amount: ${trade.amount}, Price: ${trade.price}`;

      const memory: Memory = {
        id: trade.id,
        agentId: this.runtime.agentId,
        entityId: this.runtime.agentId,
        roomId: this.runtime.agentId,
        content: {
          text: memoryContent,
          trade,
        },
        createdAt: Date.now(),
      };

      // Add embedding and store memory
      const memoryWithEmbedding = await this.runtime.addEmbeddingToMemory(memory);
      await this.runtime.createMemory(memoryWithEmbedding, 'trades', true);

      // Cache for quick access
      const cacheKey = `trade:${trade.chain}:${trade.tokenAddress}:${trade.txHash}`;
      await this.runtime.setCache(cacheKey, trade);

      logger.info(`Stored ${trade.type} trade for ${trade.tokenAddress}`);
    } catch (error) {
      logger.error(`Error storing trade for ${trade.tokenAddress}:`, error);
      throw error;
    }
  }

  async getTradesForToken(tokenAddress: string, chain: string): Promise<TradeMemory[]> {
    try {
      const memories = await this.runtime.getMemories({
        agentId: this.runtime.agentId,
        tableName: 'trades',
      });

      return memories
        .filter((memory) => {
          const trade = memory.content.trade as TradeMemory;
          return trade.tokenAddress === tokenAddress && trade.chain === chain;
        })
        .map((memory) => memory.content.trade as TradeMemory)
        .sort((a, b) => b.timestamp.getTime() - a.timestamp.getTime());
    } catch (error) {
      logger.error(`Error getting trades for token ${tokenAddress}:`, error);
      return [];
    }
  }

  async createTrade(params: {
    tokenAddress: string;
    chain: string;
    type: 'BUY' | 'SELL';
    amount: string;
    price: string;
    txHash?: string;
    metadata?: TradeMemory['metadata'];
  }): Promise<TradeMemory> {
    const trade: TradeMemory = {
      id: uuidv4() as UUID,
      timestamp: new Date(),
      ...params,
    };

    await this.storeTrade(trade);
    return trade;
  }

  async getRecentTrades(limit: number = 10): Promise<TradeMemory[]> {
    try {
      const memories = await this.runtime.getMemories({
        agentId: this.runtime.agentId,
        tableName: 'trades',
        count: limit,
      });

      // Sort after fetching
      return memories
        .sort((a, b) => {
          const tradeA = a.content.trade as TradeMemory;
          const tradeB = b.content.trade as TradeMemory;
          return tradeB.timestamp.getTime() - tradeA.timestamp.getTime();
        })
        .map((memory) => memory.content.trade as TradeMemory);
    } catch (error) {
      logger.error('Error getting recent trades:', error);
      return [];
    }
  }

  async searchTrades(query: string): Promise<TradeMemory[]> {
    try {
      // Get embedding for search query
      const queryEmbedding = await this.runtime.useModel(ModelType.TEXT_EMBEDDING, query);

      // Search memories with similar embeddings
      const memories = await this.runtime.searchMemories({
        embedding: queryEmbedding,
        tableName: 'trades',
        count: 10,
        match_threshold: 0.7,
        roomId: this.runtime.agentId,
      });

      return memories.map((memory) => memory.content.trade as TradeMemory);
    } catch (error) {
      logger.error('Error searching trades:', error);
      return [];
    }
  }

  async deleteTrade(tradeId: UUID): Promise<void> {
    try {
      await this.runtime.deleteMemory(tradeId);
      logger.info(`Deleted trade ${tradeId}`);
    } catch (error) {
      logger.error(`Error deleting trade ${tradeId}:`, error);
      throw error;
    }
  }
}

} // namespace elizaos
