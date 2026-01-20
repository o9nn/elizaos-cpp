#include ".lib/base-client.hpp"
#include ".types/entities.hpp"
#include "elizaos/core.hpp"
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



class EntitiesService extends BaseApiClient {
  /**
   * Get entity by ID
   */
  async getEntity(entityId: UUID): Promise<Entity> {
    const response = await this.get<{ entity: Entity }>(`/api/entities/${entityId}`);
    return response.entity;
  }

  /**
   * Create a new entity
   */

  /**
   * Update an existing entity
   */

  /**
   * Delete an entity
   * TODO: Uncomment when deleteEntity endpoint is fully implemented
   */
  /*
  */


} // namespace elizaos
