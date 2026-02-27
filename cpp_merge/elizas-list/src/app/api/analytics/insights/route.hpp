#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_APP_API_ANALYTICS_INSIGHTS_ROUTE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_APP_API_ANALYTICS_INSIGHTS_ROUTE_H
#include "core.hpp"
#include "next/server.h"
// External dependency removed
// External dependency removed
// External dependency removed

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
