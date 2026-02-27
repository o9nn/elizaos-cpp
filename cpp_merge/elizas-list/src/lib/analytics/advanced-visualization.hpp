#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ANALYTICS_ADVANCED_VISUALIZATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_ANALYTICS_ADVANCED_VISUALIZATION_H
#include "core.hpp"
#include "chart.js.h"
#include "d3.hpp"

class AdvancedVisualization;

class AdvancedVisualization : public object, public std::enable_shared_from_this<AdvancedVisualization> {
public:
    using std::enable_shared_from_this<AdvancedVisualization>::shared_from_this;
    static std::shared_ptr<ChartConfiguration> generateSegmentationSunburst(array<any> segments);
    static std::shared_ptr<ChartConfiguration> generatePredictionGraph(array<any> predictions);
    static any generateNetworkGraph(any data);
};

#endif
