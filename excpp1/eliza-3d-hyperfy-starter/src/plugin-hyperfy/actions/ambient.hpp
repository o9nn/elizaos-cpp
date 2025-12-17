#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::optional<std::string> getFirstAvailableField(Record<string obj, auto any>, const std::vector<std::string>& fields);

std::optional<Content> extractAmbientContent(Memory response, const std::vector<std::string>& fieldKeys);


} // namespace elizaos
