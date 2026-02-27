#include "cacheManager.hpp"

std::shared_ptr<Promise<void>> CacheManager::delete(string key)
{
    this->cache->delete(key);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CacheManager::clear()
{
    this->cache->clear();
    return std::shared_ptr<Promise<void>>();
}

