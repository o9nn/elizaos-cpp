#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Store daily summary in the database
 */
std::future<void> storeDailySummary(const std::string& username, const std::string& date, const std::string& summary, IntervalType intervalType);

/**
 * Store repository summary in the database
 */
std::future<void> storeRepoSummary(const std::string& repoId, const std::string& date, const std::string& summary, IntervalType intervalType); // --- Helper functions ---

} // namespace elizaos
