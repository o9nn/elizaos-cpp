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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<std::vector> fetchCoinsList(std::optional<AbortSignal> signal);>> {

std::optional<double> usd(const std:& obj, const std::vector<std::string>& path);

  // If caller hints a specific platform (e.g., resolved via contract endpoint), honor it first

std: platformToNet(std::optional<std:> p);

std: formatCoinMetadata(const std:& requestId, const std:& data, std::optional<std:> preferredPlatform);


} // namespace elizaos
