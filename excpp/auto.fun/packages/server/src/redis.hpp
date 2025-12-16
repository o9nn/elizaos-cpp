#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "util.hpp"

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
public:
    RedisCacheService(RedisPool public redisPool);
    std::future<bool> isPoolReady();
    std::future<double> publish(const std::string& channel, const std::string& message);
    void getKey(const std::string& key);
    std::variant<Promise<string, null>> get(const std::string& key);
    std::variant<Promise<"OK", null>> set(const std::string& key, const std::string& value, std::optional<double> ttlInSeconds);
    std::future<double> del(const std::string& key);
    std::future<bool> exists(const std::string& key);
    std::future<double> ttl(const std::string& key);
    std::future<double> lpush(const std::string& key, const std::vector<std::string>& ...values);
    std::future<std::vector<std::string>> lrange(const std::string& key, double start, double stop);
    std::future<double> llen(const std::string& key);
    std::variant<Promise<"OK", null>> ltrim(const std::string& key, double start, double stop);
    std::variant<std::future<Array<unknown>, null>> lpushTrim(const std::string& key, const std::string& value, double maxLength);
    std::future<double> sadd(const std::string& key, const std::variant<std::string, std::vector<std::string>>& member);
    std::future<double> srem(const std::string& key, const std::variant<std::string, std::vector<std::string>>& member);
    std::future<std::vector<std::string>> smembers(const std::string& key);
    void if(auto ttlInSeconds);
    std::future<bool> acquireLock(const std::string& lockKey, const std::string& lockValue, double ttlMilliseconds);
    std::future<void> defineReleaseLockScript(Redis client);
    std::future<bool> releaseLock(const std::string& lockKey, const std::string& lockValue);
};

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
public:
    RedisPool(RedisPoolOptions = {} options);
    std::future<Redis> acquire();
    std::future<void> release(Redis client);
    std::future<void> destroy();
    std::future<Redis> getPublisherClient();
    std::future<Redis> getSubscriberClient();


} // namespace elizaos
