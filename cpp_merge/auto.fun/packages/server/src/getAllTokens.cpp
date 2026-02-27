#include "getAllTokens.hpp"

std::shared_ptr<Promise<any>> getLastProcessedSlot()
{
    auto db = getDB();
    auto row = std::async([=]() { db["select"]()["from"](metadata)["where"](eq(metadata->key, std:("lastProcessedSlot")))["limit"](1); });
    if (row["length"] > 0) {
        return parseInt(const_(row)[0]["value"], 10);
    }
    return nullptr;
};


std::shared_ptr<Promise<void>> setLastProcessedSlot(double slot)
{
    auto db = std::async([=]() { getDB(); });
    std::async([=]() { db["insert"](metadata)["values"](object{
        object::pair{std:("key"), std:("lastProcessedSlot")}, 
        object::pair{std:("value"), slot->toString()}
    })["onConflictDoUpdate"](object{
        object::pair{std:("target"), array<any>{ metadata->key }}, 
        object::pair{std:("set"), object{
            object::pair{std:("value"), slot->toString()}
        }}
    }); });
    logger["log"](std:("Updated last processed slot to ") + slot + string_empty);
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
            object::pair{std:("transactionDetails"), std:("full")}, 
            object::pair{std:("rewards"), false}, 
            object::pair{std:("commitment"), std:("confirmed")}, 
            object::pair{std:("maxSupportedTransactionVersion"), 0}
        }); });
        if (!block) return logger["log"](std:("Slot ") + slot + std:(" empty, skipping"));
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
        logger["error"](std:("Error processing slot ") + slot + std:(":"), err);
    }
};


std::shared_ptr<Promise<void>> processMissedEvents(std::shared_ptr<Connection> connection)
{
    try
    {
        auto currentSlot = std::async([=]() { connection->getSlot(std:("confirmed")); });
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
            logger["log"](std:("No lastProcessedSlot found. Falling back to slot ") + startSlot + string_empty);
        } else {
            logger["log"](std:("Resuming from lastProcessedSlot = ") + startSlot + string_empty);
        }
        auto slots = std::async([=]() { connection->getBlocks(startSlot + 1, currentSlot); });
        logger["log"](std:("Processing ") + slots->length + std:(" slots from ") + (startSlot + 1) + std:(" to ") + currentSlot + string_empty);
        logger["log"](std:("Scanning events from slot ") + (startSlot + 1) + std:(" to ") + currentSlot + string_empty);
        auto queue = std::make_shared<PQueue>(object{
            object::pair{std:("concurrency"), 20}
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
        logger["log"](std:("✅ Updated lastProcessedSlot → ") + currentSlot + string_empty);
    }
    catch (const any& error)
    {
        logger["error"](std:("Error processing missed events:"), error);
    }
    return std::shared_ptr<Promise<void>>();
};


