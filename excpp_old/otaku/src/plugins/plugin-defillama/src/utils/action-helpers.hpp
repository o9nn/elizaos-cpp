#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


double parsePositiveInteger(const std::variant<std::string, double>& value);

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

std::future<ActionResult> respondWithError(HandlerCallback callback, const std::string& messageText, const std::string& errorCode, std::optional<Record<string> details, auto string | number | null>);

std::string sanitizeChainName(const std::string& value);

std::string sanitizeFilterSegment(const std::string& value);


} // namespace elizaos
