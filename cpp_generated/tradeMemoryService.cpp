#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/tradeMemoryService.h"

TradeMemoryService::TradeMemoryService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
}

std::shared_ptr<Promise<void>> TradeMemoryService::initialize()
{
    logger->info(std::string("Initializing trade memory service"));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TradeMemoryService::storeTrade(std::shared_ptr<TradeMemory> trade)
{
    try
    {
        auto memoryContent = string_empty + trade->type + std::string(" trade for ") + trade->tokenAddress + std::string(" on ") + trade->chain + std::string(" at ") + trade->timestamp->toISOString() + std::string(". Amount: ") + trade->amount + std::string(", Price: ") + trade->price + string_empty;
        auto memory = object{
            object::pair{std::string("id"), trade->id}, 
            object::pair{std::string("agentId"), this->runtime->agentId}, 
            object::pair{std::string("entityId"), this->runtime->agentId}, 
            object::pair{std::string("roomId"), this->runtime->agentId}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), memoryContent}, 
                object::pair{std::string("trade"), std::string("trade")}
            }}, 
            object::pair{std::string("createdAt"), Date->now()}
        };
        auto memoryWithEmbedding = std::async([=]() { this->runtime->addEmbeddingToMemory(memory); });
        std::async([=]() { this->runtime->createMemory(memoryWithEmbedding, std::string("trades"), true); });
        auto cacheKey = std::string("trade:") + trade->chain + std::string(":") + trade->tokenAddress + std::string(":") + trade->txHash + string_empty;
        std::async([=]() { this->runtime->setCache(cacheKey, trade); });
        logger->info(std::string("Stored ") + trade->type + std::string(" trade for ") + trade->tokenAddress + string_empty);
    }
    catch (const any& error)
    {
        logger->error(std::string("Error storing trade for ") + trade->tokenAddress + std::string(":"), error);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> TradeMemoryService::getTradesForToken(string tokenAddress, string chain)
{
    try
    {
        auto memories = std::async([=]() { this->runtime->getMemories(object{
            object::pair{std::string("agentId"), this->runtime->agentId}, 
            object::pair{std::string("tableName"), std::string("trades")}
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
        logger->error(std::string("Error getting trades for token ") + tokenAddress + std::string(":"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<std::shared_ptr<TradeMemory>>> TradeMemoryService::createTrade(object params)
{
    auto trade = utils::assign(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std::string("timestamp"), std::make_shared<Date>()}
    }, params);
    std::async([=]() { this->storeTrade(trade); });
    return trade;
}

std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> TradeMemoryService::getRecentTrades(double limit)
{
    try
    {
        auto memories = std::async([=]() { this->runtime->getMemories(object{
            object::pair{std::string("agentId"), this->runtime->agentId}, 
            object::pair{std::string("tableName"), std::string("trades")}, 
            object::pair{std::string("count"), limit}
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
        logger->error(std::string("Error getting recent trades:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> TradeMemoryService::searchTrades(string query)
{
    try
    {
        auto queryEmbedding = std::async([=]() { this->runtime->useModel(ModelType->TEXT_EMBEDDING, query); });
        auto memories = std::async([=]() { this->runtime->searchMemories(object{
            object::pair{std::string("embedding"), queryEmbedding}, 
            object::pair{std::string("tableName"), std::string("trades")}, 
            object::pair{std::string("count"), 10}, 
            object::pair{std::string("match_threshold"), 0.7}, 
            object::pair{std::string("roomId"), this->runtime->agentId}
        }); });
        return memories->map([=](auto memory) mutable
        {
            return as<std::shared_ptr<TradeMemory>>(memory["content"]->trade);
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std::string("Error searching trades:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<void>> TradeMemoryService::deleteTrade(std::shared_ptr<UUID> tradeId)
{
    try
    {
        std::async([=]() { this->runtime->deleteMemory(tradeId); });
        logger->info(std::string("Deleted trade ") + tradeId + string_empty);
    }
    catch (const any& error)
    {
        logger->error(std::string("Error deleting trade ") + tradeId + std::string(":"), error);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
}

