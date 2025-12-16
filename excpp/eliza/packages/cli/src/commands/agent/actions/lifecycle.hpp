#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..shared.hpp"
#include ".utils.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Start command implementation - starts an agent with character configuration
 */
std::future<void> startAgent(OptionValues options);

/**
 * Stop command implementation - stops a running agent
 */
std::future<void> stopAgent(OptionValues opts);

} // namespace elizaos
