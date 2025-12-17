#include "prisma.hpp"
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



std::future<void> trackSearch(const std::string& query, std::optional<std::string> userId);

std::future<void> getPopularSearches(auto limit = 10); 
} // namespace elizaos
