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



struct TimeSeriesDataPoint {
    std::string date;
    double value;
};

struct InteractionData {
};

std::future<void> GET(Request request); 
} // namespace elizaos
