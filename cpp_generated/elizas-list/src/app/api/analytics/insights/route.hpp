#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_APP_API_ANALYTICS_INSIGHTS_ROUTE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_APP_API_ANALYTICS_INSIGHTS_ROUTE_H
#include "core.h"
#include "next/server.h"
#include "@/lib/analytics/segmentation.h"
#include "@/lib/analytics/visualization.h"
#include "@/lib/monitoring/logger.h"

class TimeSeriesDataPoint;
class InteractionData;

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

any GET(std::shared_ptr<Request> request);

#endif
