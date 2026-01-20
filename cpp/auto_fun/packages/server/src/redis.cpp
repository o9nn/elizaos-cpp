#include "redis.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<RedisCacheService> getGlobalRedisCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!globalRedisCachePromise) {
        globalRedisCachePromise = (async () => {
            const auto instance = createRedisCache();
            std::cout << "[Redis] Global Redis Cache initialized." << std::endl;
            return instance;
            })();
        }
        return globalRedisCachePromise;

}

RedisPool getSharedRedisPool() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!sharedRedisPool) {
        std::cout << "Initializing Shared Redis Pool" << std::endl;
        sharedRedisPool = new RedisPool({
            host: process.env.REDIS_HOST,
            port: Number(process.env.REDIS_PORT),
            password: process.env.REDIS_PASSWORD,
            min: 50,
            max: 300,
            });

            std::cout << "Shared Redis Pool Initialized" << std::endl;
        }
        return sharedRedisPool;

}

RedisCacheService createRedisCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto pool = getSharedRedisPool();
    const auto instance = new RedisCacheService(pool);
    return instance;

}

} // namespace elizaos
