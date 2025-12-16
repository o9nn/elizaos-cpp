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
;

// Initialize Redis client
const redis = new Redis(process.env.REDIS_URL || 'redis://localhost:6379');

class CacheManager {
  static async get(key: string) {
    try {
      const cached = await redis.get(key);
      return cached ? JSON.parse(cached) : null;
    } catch (error) {
      console.error('Cache get error:', error);
      return null;
    }
  }

  static async set(key: string, data: any, ttl: number = 300) {
    try {
      await redis.setex(key, ttl, JSON.stringify(data));
    } catch (error) {
      console.error('Cache set error:', error);
    }
  }
} 
} // namespace elizaos
