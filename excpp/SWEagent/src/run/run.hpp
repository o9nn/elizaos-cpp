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
#include "agent/problem-statement.hpp"
#include "run-batch.hpp"
#include "run-replay.hpp"
#include "run-single.hpp"
#include "types.hpp"
#include "utils/files.hpp"
#include "utils/log.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Main run module CLI
 * Converted from sweagent/run/run.py
 */

/**
 * Main run function - determines which run mode to use
 */
std::future<void> run(const std::vector<std::string>& args);

// If running as main module

} // namespace elizaos
