#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/tokenSupplyHelpers/monitoring.h"

std::shared_ptr<Promise<object>> startMonitoringBatch(double batchSize)
{
    auto db = getDB();
    auto redisCache = createRedisCache();
    auto rawList = std::async([=]() { redisCache->get(std::string("lockedList")); });
    auto rawCursor = std::async([=]() { redisCache->get(std::string("lockedCursor")); });
    if (!rawList) {
        auto locked = std::async([=]() { db["select"]()["from"](tokens)["where"](eq(tokens->status, std::string("locked"))); });
        auto mints = locked["map"]([=](auto t) mutable
        {
            return t["mint"];
        }
        );
        std::async([=]() { redisCache->set(std::string("lockedList"), JSON->stringify(mints)); });
        std::async([=]() { redisCache->set(std::string("lockedCursor"), std::string("0")); });
        return object{
            object::pair{std::string("processed"), 0}, 
            object::pair{std::string("total"), mints["length"]}
        };
    }
    auto mints = JSON->parse(rawList);
    auto cursor = parseInt(OR((rawCursor), (std::string("0"))), 10);
    auto total = mints->get_length();
    if (cursor >= total) {
        return object{
            object::pair{std::string("processed"), 0}, 
            object::pair{std::string("total"), std::string("total")}
        };
    }
    auto batch = mints->slice(cursor, cursor + batchSize);
    logger["info"](std::string("Monitoring: Processing batch of ") + batch->get_length() + std::string(" tokens starting from cursor ") + cursor + std::string("."));
    for (auto& mint : batch)
    {
        try
        {
            auto ext = std::async([=]() { ExternalToken::create(mint, redisCache); });
            logger["info"](std::string("Monitoring: Successfully registered webhook for ") + mint + std::string("."));
        }
        catch (const any& err)
        {
            logger["error"](std::string("Monitoring: Failed to register webhook for ") + mint + std::string(":"), err);
        }
    }
    cursor += batch->get_length();
    logger["info"](std::string("Monitoring: Batch processed. Updating cursor to ") + cursor + std::string("."));
    std::async([=]() { redisCache->set(std::string("lockedCursor"), cursor->toString()); });
    return object{
        object::pair{std::string("processed"), batch->get_length()}, 
        object::pair{std::string("total"), std::string("total")}
    };
};


