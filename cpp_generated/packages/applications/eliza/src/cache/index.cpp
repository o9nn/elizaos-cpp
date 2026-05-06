#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-starter/src/cache/index.h"

any initializeDbCache(std::shared_ptr<Character> character, std::shared_ptr<IDatabaseCacheAdapter> db)
{
    auto cache = std::make_shared<CacheManager>(std::make_shared<DbCacheAdapter>(db, character->id));
    return cache;
};


