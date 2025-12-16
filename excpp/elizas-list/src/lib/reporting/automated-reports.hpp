#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".monitoring/logger.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ReportGenerator {
public:
    ReportGenerator();
    std::future<Buffer> generateReport(ReportData data, ReportConfig config);
    void addVisualization(jsPDF doc, const std::vector<VisualizationData>& visualizations);

private:
    ChartJSNodeCanvas chartGenerator_;
};
 
} // namespace elizaos
