#include "monitoring.hpp"

std::shared_ptr<Promise<object>> startMonitoringBatch(double batchSize)
{
    auto db = getDB();
    auto redisCache = createRedisCache();
    auto rawList = std::async([=]() { redisCache->get(std:("lockedList")); });
    auto rawCursor = std::async([=]() { redisCache->get(std:("lockedCursor")); });
    if (!rawList) {
        auto locked = std::async([=]() { db["select"]()["from"](tokens)["where"](eq(tokens->status, std:("locked"))); });
        auto mints = locked["map"]([=](auto t) mutable
        {
            return t["mint"];
        }
        );
        std::async([=]() { redisCache->set(std:("lockedList"), JSON->stringify(mints)); });
        std::async([=]() { redisCache->set(std:("lockedCursor"), std:("0")); });
        return object{
            object::pair{std:("processed"), 0}, 
            object::pair{std:("total"), mints["length"]}
        };
    }
    auto mints = JSON->parse(rawList);
    auto cursor = parseInt(OR((rawCursor), (std:("0"))), 10);
    auto total = mints->get_length();
    if (cursor >= total) {
        return object{
            object::pair{std:("processed"), 0}, 
            object::pair{std:("total"), std:("total")}
        };
    }
    auto batch = mints->slice(cursor, cursor + batchSize);
    logger["info"](std:("Monitoring: Processing batch of ") + batch->get_length() + std:(" tokens starting from cursor ") + cursor + std:("."));
    for (auto& mint : batch)
    {
        try
        {
            auto ext = std::async([=]() { ExternalToken::create(mint, redisCache); });
            logger["info"](std:("Monitoring: Successfully registered webhook for ") + mint + std:("."));
        }
        catch (const any& err)
        {
            logger["error"](std:("Monitoring: Failed to register webhook for ") + mint + std:(":"), err);
        }
    }
    cursor += batch->get_length();
    logger["info"](std:("Monitoring: Batch processed. Updating cursor to ") + cursor + std:("."));
    std::async([=]() { redisCache->set(std:("lockedCursor"), cursor->toString()); });
    return object{
        object::pair{std:("processed"), batch->get_length()}, 
        object::pair{std:("total"), std:("total")}
    };
};


