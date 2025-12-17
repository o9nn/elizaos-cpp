#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WeatherEvalResponse {
    bool isValid;
    std::string reason;
    std::optional<std::vector<std::string>> recommendations;
};

    // If we have structured weather data from an action

      // Check data freshness (within last hour)

      // Generate condition-based recommendations

    // For text-based responses from provider


} // namespace elizaos
