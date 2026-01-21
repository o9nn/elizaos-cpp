#include "rateLimiter.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void createRateLimiter(RedisCacheService redisCache) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return rateLimiter({;
        windowMs: WINDOW_SEC * 1000,
        limit: 250,
        standardHeaders: "draft-7",
        keyGenerator: (c: Context) =>
        c.req.header("x-forwarded-for") ||;
        c.req.header("cf-connecting-ip") ||;
        "unknown",
        store: new (class {
            async increment(key: string) {
                const auto full = "redisCache.getKey(" + "rl:" + key;
                const auto [[, count]] = (redisCache.redisPool.useClient((client) =>;
                client.multi().incr(full).expire(full, WINDOW_SEC).exec();
                ))<[Error | nullptr, number]>;
                return {
                    totalHits: count,
                    resetTime: new Date(Date.now() + WINDOW_SEC * 1000),
                    };
                }
                async decrement(key: string) {
                    const auto full = "redisCache.getKey(" + "rl:" + key;
                    redisCache.redisPool.useClient((client) => client.decr(full));
                }
                async resetKey(key: string) {
                    const auto full = "redisCache.getKey(" + "rl:" + key;
                    redisCache.del(full);
                }
                })(),
                });

}

} // namespace elizaos
