#include "elizaos/core.hpp"
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



std::future<any | null> safeReadJson(Response res);

std::future<std::vector> fetchCoinsList(std::optional<AbortSignal> signal);>> {

void choosePlatform(const std::any& data, std::optional<string | null> preferredPlatform); {
  // If caller hints a specific platform (e.g., resolved via contract endpoint), honor it first

std::string platformToNet(std::optional<string | null> p);

std::any formatCoinMetadata(const std::string& requestId, const std::any& data, std::optional<string | null> preferredPlatform);


} // namespace elizaos
