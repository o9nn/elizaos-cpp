#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


double parsePositiveInteger(const std::optional<double>& value);

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


std::string sanitizeChainName(const std::string& value);

std::string sanitizeFilterSegment(const std::string& value);


} // namespace elizaos
