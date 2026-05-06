#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "monitoring/logger.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TimeSeriesDecomposer {
public:
    TimeSeriesDecomposer();
    std::future<DecompositionResult> decompose(const std::vector<double>& timeSeries, DecompositionConfig config);
    std::future<STLResult> performSTLDecomposition(const std::vector<double>& timeSeries, DecompositionConfig config);
    std::future<WaveletResult> performWaveletDecomposition(const std::vector<double>& timeSeries, DecompositionConfig config);
    std::future<EMDResult> performEMD(const std::vector<double>& timeSeries);
    std::future<std::vector<Changepoint>> detectChangepoints(const std::vector<double>& timeSeries, DecompositionComponents components);
    std::future<ComponentForecasts> generateComponentForecasts(DecompositionComponents components, DecompositionConfig config);
};
 
} // namespace elizaos
