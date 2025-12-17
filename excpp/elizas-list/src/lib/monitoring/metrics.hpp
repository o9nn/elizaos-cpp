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

class MetricsService {
public:
    static void recordApiLatency(const std::string& path, double latency);
    static void recordProjectView(const std::string& projectId, std::optional<std::string> userId);
    static void getMetrics();
};
 
} // namespace elizaos
