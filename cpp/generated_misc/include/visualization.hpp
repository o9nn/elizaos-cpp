#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYTICS_VISUALIZATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_ANALYTICS_VISUALIZATION_H
#include "core.h"
#include "chart.js.h"

class TimeSeriesDataPoint;
class InteractionData;
class AnalyticsVisualization;

class TimeSeriesDataPoint : public object, public std::enable_shared_from_this<TimeSeriesDataPoint> {
public:
    using std::enable_shared_from_this<TimeSeriesDataPoint>::shared_from_this;
    string date;

    double value;
};

class InteractionData : public object, public std::enable_shared_from_this<InteractionData> {
public:
    using std::enable_shared_from_this<InteractionData>::shared_from_this;
};

class AnalyticsVisualization : public object, public std::enable_shared_from_this<AnalyticsVisualization> {
public:
    using std::enable_shared_from_this<AnalyticsVisualization>::shared_from_this;
    static std::shared_ptr<ChartConfiguration> generateTimeSeriesChart(array<std::shared_ptr<TimeSeriesDataPoint>> data);
    static std::shared_ptr<ChartConfiguration> generateHeatmap(std::shared_ptr<InteractionData> data);
};

#endif
