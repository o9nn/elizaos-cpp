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



class RateLimiter {
  static async checkLimit(key: string, limit: number = 100, window: number = 3600): Promise<boolean> {
    const current = await rateLimitRedis.incr(key);
    if (current === 1) {
      await rateLimitRedis.expire(key, window);
    }
    return current <= limit;
  }

} // namespace elizaos
