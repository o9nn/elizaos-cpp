#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void initializeDbCache(Character character, IDatabaseCacheAdapter db) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cache = new CacheManager(new DbCacheAdapter(db, character.id));
    return cache;

}

} // namespace elizaos
