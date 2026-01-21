#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "config.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Call AI service to generate a summary
 */

std::future<std::string> callAIService(const std::string& prompt, AISummaryConfig config, std::optional<std::any> options);

} // namespace elizaos
