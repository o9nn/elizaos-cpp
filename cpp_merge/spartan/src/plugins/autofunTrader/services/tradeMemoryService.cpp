#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofunTrader/services/tradeMemoryService.h"

TradeMemoryService::TradeMemoryService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
}

std::shared_ptr<Promise<void>> TradeMemoryService::initialize()
{
    logger->info(std:("Initializing trade memory service"));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TradeMemoryService::storeTrade(std::shared_ptr<TradeMemory> trade)
{
    try
    {
        auto memoryContent = string_empty + trade->type + std:(" trade for ") + trade->tokenAddress + std:(" on ") + trade->chain + std:(" at ") + trade->timestamp->toISOString() + std:(". Amount: ") + trade->amount + std:(", Price: ") + trade->price + string_empty;
        auto memory = object{
            object::pair{std:("id"), trade->id}, 
            object::pair{std:("agentId"), this->runtime->agentId}, 
            object::pair{std:("entityId"), this->runtime->agentId}, 
            object::pair{std:("roomId"), this->runtime->agentId}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), memoryContent}, 
                object::pair{std:("trade"), std:("trade")}
            }}, 
            object::pair{std:("createdAt"), Date->now()}
        };
        auto memoryWithEmbedding = std::async([=]() { this->runtime->addEmbeddingToMemory(memory); });
        std::async([=]() { this->runtime->createMemory(memoryWithEmbedding, std:("trades"), true); });
        auto cacheKey = std:("trade:") + trade->chain + std:(":") + trade->tokenAddress + std:(":") + trade->txHash + string_empty;
        std::async([=]() { this->runtime->setCache(cacheKey, trade); });
        logger->info(std:("Stored ") + trade->type + std:(" trade for ") + trade->tokenAddress + string_empty);
    }
    catch (const any& error)
    {
        logger->error(std:("Error storing trade for ") + trade->tokenAddress + std:(":"), error);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> TradeMemoryService::getTradesForToken(string tokenAddress, string chain)
{
    try
    {
        auto memories = std::async([=]() { this->runtime->getMemories(object{
            object::pair{std:("agentId"), this->runtime->agentId}, 
            object::pair{std:("tableName"), std:("trades")}
        }); });
        return memories->filter([=](auto memory) mutable
        {
            auto trade = as<std::shared_ptr<TradeMemory>>(memory["content"]["trade"]);
            return AND((trade->tokenAddress == tokenAddress), (trade->chain == chain));
        }
        )->map([=](auto memory) mutable
        {
            return as<std::shared_ptr<TradeMemory>>(memory["content"]->trade);
        }
        )->sort([=](auto a, auto b) mutable
        {
            return b["timestamp"]->getTime() - a["timestamp"]->getTime();
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std:("Error getting trades for token ") + tokenAddress + std:(":"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<std::shared_ptr<TradeMemory>>> TradeMemoryService::createTrade(object params)
{
    auto trade = utils::assign(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std:("timestamp"), std::make_shared<Date>()}
    }, params);
    std::async([=]() { this->storeTrade(trade); });
    return trade;
}

std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> TradeMemoryService::getRecentTrades(double limit)
{
    try
    {
        auto memories = std::async([=]() { this->runtime->getMemories(object{
            object::pair{std:("agentId"), this->runtime->agentId}, 
            object::pair{std:("tableName"), std:("trades")}, 
            object::pair{std:("count"), limit}
        }); });
        return memories->sort([=](auto a, auto b) mutable
        {
            auto tradeA = as<std::shared_ptr<TradeMemory>>(a["content"]["trade"]);
            auto tradeB = as<std::shared_ptr<TradeMemory>>(b["content"]["trade"]);
            return tradeB->timestamp->getTime() - tradeA->timestamp->getTime();
        }
        )->map([=](auto memory) mutable
        {
            return as<std::shared_ptr<TradeMemory>>(memory["content"]->trade);
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std:("Error getting recent trades:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> TradeMemoryService::searchTrades(string query)
{
    try
    {
        auto queryEmbedding = std::async([=]() { this->runtime->useModel(ModelType->TEXT_EMBEDDING, query); });
        auto memories = std::async([=]() { this->runtime->searchMemories(object{
            object::pair{std:("embedding"), queryEmbedding}, 
            object::pair{std:("tableName"), std:("trades")}, 
            object::pair{std:("count"), 10}, 
            object::pair{std:("match_threshold"), 0.7}, 
            object::pair{std:("roomId"), this->runtime->agentId}
        }); });
        return memories->map([=](auto memory) mutable
        {
            return as<std::shared_ptr<TradeMemory>>(memory["content"]->trade);
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std:("Error searching trades:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<void>> TradeMemoryService::deleteTrade(std::shared_ptr<UUID> tradeId)
{
    try
    {
        std::async([=]() { this->runtime->deleteMemory(tradeId); });
        logger->info(std:("Deleted trade ") + tradeId + string_empty);
    }
    catch (const any& error)
    {
        logger->error(std:("Error deleting trade ") + tradeId + std:(":"), error);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
}

