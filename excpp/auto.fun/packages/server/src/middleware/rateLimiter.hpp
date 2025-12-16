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
import type { Context } from "hono";
import type { RedisCacheService } from "../redis";

const WINDOW_SEC = 10;

`);
        const [[, count]] = (await redisCache.redisPool.useClient((client) =>
          client.multi().incr(full).expire(full, WINDOW_SEC).exec()
        )) as Array<[Error | null, number]>;
        return {
          totalHits: count,
          resetTime: new Date(Date.now() + WINDOW_SEC * 1000),
        };
      }
      async decrement(key: string) {
        const full = redisCache.getKey(`rl:${key}`);
        await redisCache.redisPool.useClient((client) => client.decr(full));
      }
      async resetKey(key: string) {
        const full = redisCache.getKey(`rl:${key}`);
        await redisCache.del(full);
      }
    })(),
  });
}

} // namespace elizaos
