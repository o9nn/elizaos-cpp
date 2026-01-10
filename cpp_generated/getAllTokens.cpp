#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/getAllTokens.h"

std::shared_ptr<Promise<any>> getLastProcessedSlot()
{
    auto db = getDB();
    auto row = std::async([=]() { db["select"]()["from"](metadata)["where"](eq(metadata->key, std::string("lastProcessedSlot")))["limit"](1); });
    if (row["length"] > 0) {
        return parseInt(const_(row)[0]["value"], 10);
    }
    return nullptr;
};


std::shared_ptr<Promise<void>> setLastProcessedSlot(double slot)
{
    auto db = std::async([=]() { getDB(); });
    std::async([=]() { db["insert"](metadata)["values"](object{
        object::pair{std::string("key"), std::string("lastProcessedSlot")}, 
        object::pair{std::string("value"), slot->toString()}
    })["onConflictDoUpdate"](object{
        object::pair{std::string("target"), array<any>{ metadata->key }}, 
        object::pair{std::string("set"), object{
            object::pair{std::string("value"), slot->toString()}
        }}
    }); });
    logger["log"](std::string("Updated last processed slot to ") + slot + string_empty);
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<double>> findSlotAtOrBeforeTime(std::shared_ptr<Connection> connection, double targetTs, double low, double high)
{
    while (low < high)
    {
        auto mid = Math->floor((low + high + 1) / 2);
        auto t = std::async([=]() { connection->getBlockTime(mid); });
        if (OR((t == nullptr), (t > targetTs))) {
            high = mid - 1;
        } else {
            low = mid;
        }
    }
    return low;
};


any processSlot(double slot, std::shared_ptr<Connection> connection)
{
    try
    {
        auto block = std::async([=]() { connection->getBlock(slot, object{
            object::pair{std::string("transactionDetails"), std::string("full")}, 
            object::pair{std::string("rewards"), false}, 
            object::pair{std::string("commitment"), std::string("confirmed")}, 
            object::pair{std::string("maxSupportedTransactionVersion"), 0}
        }); });
        if (!block) return logger["log"](std::string("Slot ") + slot + std::string(" empty, skipping"));
        for (auto& tx : block->transactions)
        {
            auto logs = tx->meta->logMessages;
            if (!logs) continue;
            if (logs->some([=](auto l) mutable
            {
                return l["includes"](process->env->PROGRAM_ID);
            }
            )) {
                auto signature = const_(tx->transaction->signatures)[0];
                std::async([=]() { processTransactionLogs(logs, signature); });
            }
        }
    }
    catch (const any& err)
    {
        logger["error"](std::string("Error processing slot ") + slot + std::string(":"), err);
    }
};


std::shared_ptr<Promise<void>> processMissedEvents(std::shared_ptr<Connection> connection)
{
    try
    {
        auto currentSlot = std::async([=]() { connection->getSlot(std::string("confirmed")); });
        auto currentTime = std::async([=]() { connection->getBlockTime(currentSlot); });
        auto startSlot = std::async([=]() { getLastProcessedSlot(); });
        if (startSlot == nullptr) {
            auto currentTime = std::async([=]() { connection->getBlockTime(currentSlot); });
            if (currentTime != nullptr) {
                auto cutoffTs = currentTime - 6 * 3600;
                startSlot = std::async([=]() { findSlotAtOrBeforeTime(connection, cutoffTs, 0, currentSlot); });
            } else {
                startSlot = Math->max(0, currentSlot - 500);
            }
            logger["log"](std::string("No lastProcessedSlot found. Falling back to slot ") + startSlot + string_empty);
        } else {
            logger["log"](std::string("Resuming from lastProcessedSlot = ") + startSlot + string_empty);
        }
        auto slots = std::async([=]() { connection->getBlocks(startSlot + 1, currentSlot); });
        logger["log"](std::string("Processing ") + slots->length + std::string(" slots from ") + (startSlot + 1) + std::string(" to ") + currentSlot + string_empty);
        logger["log"](std::string("Scanning events from slot ") + (startSlot + 1) + std::string(" to ") + currentSlot + string_empty);
        auto queue = std::make_shared<PQueue>(object{
            object::pair{std::string("concurrency"), 20}
        });
        for (auto& slot : slots)
        {
            queue->add([=]() mutable
            {
                return processSlot(slot, connection);
            }
            );
        }
        std::async([=]() { queue->onIdle(); });
        std::async([=]() { setLastProcessedSlot(currentSlot); });
        logger["log"](std::string("✅ Updated lastProcessedSlot → ") + currentSlot + string_empty);
    }
    catch (const any& error)
    {
        logger["error"](std::string("Error processing missed events:"), error);
    }
    return std::shared_ptr<Promise<void>>();
};


