#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "db.hpp"
#include "redis.hpp"
#include "util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Unified cache system using Drizzle/D1 for all caching needs
 * Simplifies the architecture by avoiding additional services like KV
 */
class CacheService {
public:
    CacheService();
    void onModuleStart();
    void initialize();
    std::variant<Promise<number, null>> getSolPrice();
    std::future<void> setSolPrice(double price, number = 30 ttlSeconds);
    std::variant<Promise<number, null>> getTokenPrice(const std::string& mint);
    std::future<void> setTokenPrice(const std::string& mint, double price, number = 300 ttlSeconds);
    std::future<void> setMetadata(const std::string& key, const std::any& data, number = 3600 ttlSeconds);
    void if(auto cachedData.length > 0);
    void catch(auto error);
    std::future<void> cleanupOldCacheEntries(const std::string& type, const std::string& symbol);

private:
    ReturnType<typeof getDB> db_;
};


} // namespace elizaos
