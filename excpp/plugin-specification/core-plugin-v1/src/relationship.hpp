#include "elizaos/core-plugin-v2.hpp"
#include "types.hpp"
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

;
;

/**
 * Relationship interface for v1 compatibility
 */
using Relationship = RelationshipFromTypes;

/**
 * Converts V2 Relationship to V1 compatible Relationship
 * Main differences:
 * - V2 uses sourceEntityId/targetEntityId, V1 uses userA/userB
 * - V2 has tags array and metadata object, V1 has status string
 * - V1 has userId and roomId fields that don't exist in V2
 */
;
}

/**
 * Converts V1 Relationship to V2 compatible Relationship
 * Maps V1 relationship structure to V2 format
 */
,
    createdAt: relationship.createdAt,
  };
}

/**
 * Converts an array of V2 Relationships to V1 format
 */


/**
 * Converts an array of V1 Relationships to V2 format
 */


/**
 * Relationship status constants commonly used in V1
 * These can be used to standardize relationship statuses
 */
const RELATIONSHIP_STATUSES = {
  FRIEND: 'friend',
  BLOCKED: 'blocked',
  MUTED: 'muted',
  FOLLOWING: 'following',
  FOLLOWED_BY: 'followed_by',
  ACQUAINTANCE: 'acquaintance',
  UNKNOWN: 'unknown',
} as const;

/**
 * Converts V2 tags to V1 status string with common mappings
 * Provides more intelligent conversion than simple join
 */
;

  // Check for known status mappings first
  for (const tag of tags) {
    const normalizedTag = tag.toLowerCase();
    if (statusMap[normalizedTag]) {
      return statusMap[normalizedTag];
    }
  }

  // If no known mapping, join all tags
  return tags.join(',');
}

/**
 * Converts V1 status string to V2 tags array with common mappings
 * Provides more intelligent conversion than simple split
 */
;

  const normalizedStatus = status.toLowerCase();
  if (tagMap[normalizedStatus]) {
    return tagMap[normalizedStatus];
  }

  // If no known mapping, split by comma
  return status.split(',').map(s => s.trim()).filter(Boolean);
}

/**
 * Enhanced conversion with intelligent status mapping
 */
;
}

/**
 * Enhanced conversion with intelligent tag mapping
 */
,
    createdAt: relationship.createdAt,
  };
}

/**
 * Utility functions for relationship management
 */

/**
 * Creates a new V1 relationship with default values
 */
-${userB}-${Date.now()}` as UUID,
    userA,
    userB,
    userId: userA,
    roomId: roomId || userA, // Use userA as fallback roomId
    status,
    createdAt: new Date().toISOString(),
  };
}

/**
 * Checks if two relationships represent the same connection (bidirectional)
 */


/**
 * Filters relationships by status
 */


/**
 * Gets all relationships for a specific user (where user is either userA or userB)
 */

} // namespace elizaos
