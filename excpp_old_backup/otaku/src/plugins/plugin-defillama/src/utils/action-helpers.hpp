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
 * Intelligently downsample time series data to reduce context size while preserving trends.
 * Uses adaptive sampling based on data length.
 */

  // Always include first point

  // Sample intermediate points at regular intervals

  // Always include last point (most recent)

/**
 * Calculate summary statistics for TVL series to reduce context size
 */

  // Find ATH (All-Time High) and its date

std::future<ActionResult> respondWithError(HandlerCallback | undefined callback, const std::string& messageText, const std::string& errorCode, std::optional<Record<string> details, auto string | number | null>);


} // namespace elizaos
