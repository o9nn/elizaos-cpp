#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Provider that produces a prompt to help the agent decide if someone should be timed out.
 * It does not analyze history directly — another provider must supply the conversation context.
 * This provider simply provides a high-level prompt instructing the agent to take the TIMEOUT_USER action if needed.
 */


} // namespace elizaos
