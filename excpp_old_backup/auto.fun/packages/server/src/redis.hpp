#include "util.hpp"
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
    // logger.info(`LPUSH ${values.length} values to ${this.getKey(key)}`);

    // logger.info(`LRANGE from ${this.getKey(key)} ${start} ${stop}`);

    // logger.info(`LLEN for ${this.getKey(key)}`);

    // logger.info(`LTRIM on ${this.getKey(key)} ${start} ${stop}`);

    // logger.info(
    //   `LPUSH+LTRIM pipeline on ${this.getKey(key)} limit ${maxLength}`
    // );
  // --- END NEW LIST METHODS ---

  // --- START NEW SET METHODS ---
    // logger.info(`SADD to ${this.getKey(key)}`);
    // Note: ioredis sadd returns number of elements added

    // logger.info(`SREM from ${this.getKey(key)}`);
    // Note: ioredis srem returns number of elements removed

    // logger.info(`SMEMBERS for ${this.getKey(key)}`);

  // Expose useClient for transactions if absolutely necessary, but prefer specific methods
  // Only uncomment if the MULTI logic cannot be encapsulated here.
  // async useClient<T>(fn: (client: Redis) => Promise<T>): Promise<T> {
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
    std::optional<std::string> host;
    std::optional<double> port;
    std::optional<std::string> password;
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
      host: options.host || process.env.REDIS_HOST || DEFAULT_REDIS_HOST,
      port:
        options.port || Number(process.env.REDIS_PORT) || DEFAULT_REDIS_PORT,
      password:
        options.password ||
        process.env.REDIS_PASSWORD ||
        DEFAULT_REDIS_PASSWORD,
      max: options.max || 500,
      min: options.min || 200,
      idleTimeoutMillis: options.idleTimeoutMillis || 60_000,
    };

    logger.info(
      `[RedisPool] Initializing with host: ${this.options.host === DEFAULT_REDIS_HOST ? "DEFAULT LOCAL HOST" : this.options.host}:${this.options.port}`
    );

    this.pool = createPool<Redis>(
      {
        create: async () => {
          const client = new Redis({
            host: this.options.host,
            port: this.options.port,
            password: this.options.password || undefined, // Pass undefined if no password
            retryStrategy: (attempts) => Math.min(attempts * 50, 2000),
            maxRetriesPerRequest: 3,
            connectTimeout: 3000,
            enableReadyCheck: true,
          });

          client.on("error", (err) => console.error("Redis Client Error", err));
          client.on("connect", () => console.log("Redis Client Connected"));
          client.on("ready", () => console.log("Redis Client Ready"));

          return client;
        },
        destroy: async (client: Redis) => {
          await client.quit();
        },
        validate: async (client: Redis) => {
          try {
            await client.ping();
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
