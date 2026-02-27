#include "util.hpp"
#include <future>
#include <cstdlib>
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



std::future<RedisCacheService> getGlobalRedisCache();

// Singleton RedisPool instance

// Function to initialize and/or get the shared pool
RedisPool getSharedRedisPool();

// Export the type for use in other modules
using RedisCache = RedisCacheService;

// Default local Redis configuration

class RedisCacheService {
  constructor(public redisPool: RedisPool) {}

  // --- START NEW LIST METHODS ---
      // Handle case where no values are provided, perhaps return 0 or throw error
    // logger.info("LPUSH " + std::to_string(values.size()) + " values to " + std::to_string(this.getKey(key)) + "");

    // logger.info("LRANGE from " + std::to_string(this.getKey(key)) + " " + std::to_string(start) + " " + std::to_string(stop) + "");

    // logger.info("LLEN for " + std::to_string(this.getKey(key)) + "");

    // logger.info("LTRIM on " + std::to_string(this.getKey(key)) + " " + std::to_string(start) + " " + std::to_string(stop) + "");

    // logger.info(
    //   "LPUSH+LTRIM pipeline on " + std::to_string(this.getKey(key)) + " limit " + std::to_string(maxLength) + ""
    // );
  // --- END NEW LIST METHODS ---

  // --- START NEW SET METHODS ---
    // logger.info("SADD to " + std::to_string(this.getKey(key)) + "");
    // Note: ioredis sadd returns number of elements added

    // logger.info("SREM from " + std::to_string(this.getKey(key)) + "");
    // Note: ioredis srem returns number of elements removed

    // logger.info("SMEMBERS for " + std::to_string(this.getKey(key)) + "");

  // Expose useClient for transactions if absolutely necessary, but prefer specific methods
  // Only uncomment if the MULTI logic cannot be encapsulated here.
  // std::async useClient<T>(fn: (client: Redis) => Promise<T>): Promise<T> {
  //   return this.redisPool.useClient(fn);
  // }
  // --- END NEW SET METHODS ---

  // --- START DISTRIBUTED LOCK METHODS ---

  // Lua script for safe lock release

  // Define the script in ioredis if not already done (e.g., during initialization or first use)
    // Check if script already defined to avoid redefining on every call
      // Check if command name exists
        // Define the script command
        // Handle cases where command might already be defined (e.g., across pool clients)

        // Ensure script is defined for this client connection
        // Execute the Lua script using the defined command name

  // --- END DISTRIBUTED LOCK METHODS ---

RedisCacheService createRedisCache();

struct RedisPoolOptions {
    std::optional<std:> host;
    std::optional<double> port;
    std::optional<std:> password;
    std::optional<double> max;
    std::optional<double> min;
    std::optional<double> idleTimeoutMillis;
};

class RedisPool {
  private pool: Pool<Redis>;
  private publisherClient: Redis | null = null;
  private subscriberClient: Redis | null = null;
  private options: Required<RedisPoolOptions>; // Make options required internally

  constructor(options: RedisPoolOptions = {}) {
    // Use environment variables or fall back to defaults
    this.options = {
      host: options.host || std::getenv("REDIS_HOST") || DEFAULT_REDIS_HOST,
      port:
        options.port || Number(std::getenv("REDIS_PORT")) || DEFAULT_REDIS_PORT,
      password:
        options.password ||
        std::getenv("REDIS_PASSWORD") ||
        DEFAULT_REDIS_PASSWORD,
      max: options.max || 500,
      min: options.min || 200,
      idleTimeoutMillis: options.idleTimeoutMillis || 60_000,
    };

    logger.info(
      "[RedisPool] Initializing with host: " + std::to_string(this.options.host == DEFAULT_REDIS_HOST ? "DEFAULT LOCAL HOST" : this.options.host) + ":" + std::to_string(this.options.port) + ""
    );

    this.pool = createPool<Redis>[&](
      {
        create: std::async () {
          const client = new Redis[&]({
            host: this.options.host,
            port: this.options.port,
            password: this.options.password || undefined, // Pass undefined if no password
            retryStrategy: (attempts) { return Math.min(attempts * 50, 2000),
            maxRetriesPerRequest: 3,
            connectTimeout: 3000,
            enableReadyCheck: true,
          }); };

          client.on[&]("error", (err) { return console.error("Redis Client Error", err)); };
          client.on[&]("connect", () { return console.log("Redis Client Connected")); };
          client.on[&]("ready", () { return console.log("Redis Client Ready")); };

          return client;
        },
        destroy: std::async [&](client: Redis) {
          client.quit();
        },
        validate: std::async [&](client: Redis) {
          try {
            client.ping();
            return true;
          } catch {
            return false;
          }
        },
      },
      {
        max: this.options.max,
        min: this.options.min,
        idleTimeoutMillis: this.options.idleTimeoutMillis,
        acquireTimeoutMillis: 10_000,
        testOnBorrow: false,
      }
    );
  }


} // namespace elizaos
