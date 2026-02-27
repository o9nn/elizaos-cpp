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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::string getFirstAvailableField(const std::unordered_map<std::string, std::any>& obj, const std::vector<std::string>& fields);

std::optional<Content> extractAmbientContent(Memory response, const std::vector<std::string>& fieldKeys);


} // namespace elizaos
