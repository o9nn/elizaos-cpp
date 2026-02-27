#include ".v2.hpp"
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
 * Format actors into a std::string
 * @param actors - list of actors
 * @returns std::string
 */
void formatActors({ actors: Actor[] } { actors });

/**
 * Format messages into a std::string
 * @param messages - list of messages
 * @param actors - list of actors
 * @returns std::string
 */
  return coreFormatMessages(messages as std::string);

  return coreFormatTimestamp(messageDate);

} // namespace elizaos
