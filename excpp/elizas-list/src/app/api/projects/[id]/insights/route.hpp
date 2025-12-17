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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct InsightsResponse {
    std::any views;
    std::any interactions;
    std::vector<std::any> timeSeriesData;
    std::any abTestResults;
    std::any timeSeriesChart;
    std::any engagementHeatmap;
};

std::future<void> GET(const std::string& request); 
} // namespace elizaos
