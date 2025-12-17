#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "shared.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Type-safe utilities for handling Tauri responses to avoid 'as any' casts
 */

/**
 * Safely extracts memories from a TauriMemoryResponse
 */
std::vector<MemoryEntry> extractMemoriesFromResponse(const std::any& response);

/**
 * Safely extracts logs from a TauriMemoryResponse
 */
std::vector<LogEntry> extractLogsFromResponse(const std::any& response);

/**
 * Type guard to check if response is a successful TauriMemoryResponse
 */

/**
 * Safely extracts data from a TauriMemoryResponse with proper typing
 */

  // If no specific key requested, try to return the data itself if it's an array

/**
 * Type-safe converter from unknown response to expected shape
 * Replaces the need for 'as unknown as Record<string, unknown>[]'
 */

} // namespace elizaos
