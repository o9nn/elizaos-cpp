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



struct InsightsResponse {
    { insights;
    std::any views;
    std::any interactions;
    { trends;
    std::vector<std::any> timeSeriesData;
    std::any abTestResults;
    std::unordered_map<std::string, std::string> realtimeStats;
    { visualizations;
    std::any timeSeriesChart;
    std::any engagementHeatmap;
};

std::future<void> GET(Request request, { params: { id: string } } { params }); 
} // namespace elizaos
