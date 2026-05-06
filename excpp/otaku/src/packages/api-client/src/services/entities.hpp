#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "lib/base-client.hpp"
#include "types/entities.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class EntitiesService {
public:
    std::future<Entity> getEntity(UUID entityId);
    std::future<Entity> createEntity(EntityCreateParams params);
    std::future<Entity> updateEntity(UUID entityId, EntityUpdateParams params);
    Promise< deleteEntity(UUID entityId);
};


} // namespace elizaos
