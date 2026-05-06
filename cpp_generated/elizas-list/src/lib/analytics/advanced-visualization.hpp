#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYTICS_ADVANCED-VISUALIZATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYTICS_ADVANCED-VISUALIZATION_H
#include "core.h"
#include "chart.js.h"
#include "d3.h"

class AdvancedVisualization;

class AdvancedVisualization : public object, public std::enable_shared_from_this<AdvancedVisualization> {
public:
    using std::enable_shared_from_this<AdvancedVisualization>::shared_from_this;
    static std::shared_ptr<ChartConfiguration> generateSegmentationSunburst(array<any> segments);
    static std::shared_ptr<ChartConfiguration> generatePredictionGraph(array<any> predictions);
    static any generateNetworkGraph(any data);
};

#endif
