#include "spartan/src/plugins/degenTrader/utils/cacheManager.h"

std::shared_ptr<Promise<void>> CacheManager::delete(std::string key)
{
    this->cache->delete(key);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> CacheManager::clear()
{
    this->cache->clear();
    return std::shared_ptr<Promise<void>>();
}

