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



struct ChartProps {
    std::vector<std::any> data;
    'network' | 'timeline' | 'heatmap' | 'force' type;
    double width;
    double height;
    std::optional<ChartOptions> options;
    std::optional<(node: any) => void> onNodeClick;
};

  // Additional rendering functions for other chart types...

} // namespace elizaos
