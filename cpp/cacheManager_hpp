#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_UTILS_CACHEMANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_UTILS_CACHEMANAGER_H
#include "core.h"

template <typename T>
class CacheEntry;
class CacheManager;

template <typename T>
class CacheEntry : public object, public std::enable_shared_from_this<CacheEntry<T>> {
public:
    using std::enable_shared_from_this<CacheEntry<T>>::shared_from_this;
    T value;

    double timestamp;

    double expiry;
};

class CacheManager : public object, public std::enable_shared_from_this<CacheManager> {
public:
    using std::enable_shared_from_this<CacheManager>::shared_from_this;
    std::shared_ptr<Map<string, std::shared_ptr<CacheEntry<any>>>> cache = std::make_shared<Map>();

    double defaultTTL = 60000;

    template <typename T>
    std::shared_ptr<Promise<any>> get(string key);
    template <typename T>
    std::shared_ptr<Promise<void>> set(string key, T value, double ttl = this->defaultTTL);
    virtual std::shared_ptr<Promise<void>> delete(string key);
    virtual std::shared_ptr<Promise<void>> clear();
};

template <typename T>
std::shared_ptr<Promise<any>> CacheManager::get(string key)
{
    auto entry = this->cache->get(key);
    if (!entry) return nullptr;
    if (Date->now() > entry->expiry) {
        this->cache->delete(key);
        return nullptr;
    }
    return as<T>(entry->value);
}

template <typename T>
std::shared_ptr<Promise<void>> CacheManager::set(string key, T value, double ttl)
{
    auto entry = object{
        object::pair{std::string("value"), std::string("value")}, 
        object::pair{std::string("timestamp"), Date->now()}, 
        object::pair{std::string("expiry"), Date->now() + ttl}
    };
    this->cache->set(key, entry);
    return std::shared_ptr<Promise<void>>();
}

#endif
