#include "elizaos/core-plugin-v2.hpp"
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
 * Get details for a list of actors.
 */

/**
 * Format actors into a string
 * @param actors - list of actors
 * @returns string
 */
void formatActors({ actors: Actor[] } { actors });

/**
 * Format messages into a string
 * @param messages - list of messages
 * @param actors - list of actors
 * @returns string
 */
  return coreFormatMessages(messages as any);

  return coreFormatTimestamp(messageDate);

} // namespace elizaos
