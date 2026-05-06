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



std::future<void> getUserTags(const std::string& username);
/**
 * Get all summaries for a user based on the interval type.
 * @param username - GitHub username of the user
 * @param intervalType - 'month' or 'week'
 * @returns Array of summaries ordered by date descending
 */
std::future<void> getUserSummaries(const std::string& username, IntervalType intervalType, std::optional<double> limit);

// Define an extended type for the profile page data
// This combines your existing return type with new wallet fields
using UserProfileData = NonNullable<

std::future<void> getUserProfile(const std::string& username);

} // namespace elizaos
