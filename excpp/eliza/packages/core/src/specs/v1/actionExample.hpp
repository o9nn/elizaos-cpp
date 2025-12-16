#include ".v2.hpp"
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



/**
 * Example content with associated user for demonstration purposes
 * This is exported from types.ts in v1, but we're recreating it here for the adapter
 */
using ActionExample = ActionExampleFromTypes;

/**
 * Safely converts a V2 content object to a V1 Content type
 * Maps known properties and preserves additional ones
 *
 * @param content V2 content object
 * @returns Content compatible with V1
 */
Content convertContentToV1(ContentV2 content);

/**
 * Safely converts a V1 Content object to a V2 compatible content type
 * Maps known properties and preserves additional ones
 *
 * @param content V1 Content object
 * @returns Content compatible with V2
 */
ContentV2 convertContentToV2(Content content);

/**
 * Converts v2 ActionExample to v1 compatible ActionExample
 *
 * @param exampleV2 The V2 action example to convert
 * @returns V1 compatible ActionExample
 */
ActionExample fromV2ActionExample(ActionExampleV2 exampleV2);

/**
 * Converts v1 ActionExample to v2 ActionExample
 *
 * @param example The V1 action example to convert
 * @returns V2 compatible ActionExample
 */
ActionExampleV2 toV2ActionExample(ActionExample example);

} // namespace elizaos
