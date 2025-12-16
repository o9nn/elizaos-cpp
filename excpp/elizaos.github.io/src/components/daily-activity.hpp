#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define the shape of our chart data
struct ChartDataItem {
    std::string date;
    std::string day;
    double prScore;
    double issueScore;
    double reviewScore;
    double commentScore;
    double total;
    bool isEmpty;
};

  // Get the most recent days of activity (up to 31 days)

  // Define chart config

  // Transform data for recharts

                // verticalAlign="top"

} // namespace elizaos
