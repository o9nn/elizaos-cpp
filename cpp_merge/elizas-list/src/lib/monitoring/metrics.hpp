#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_MONITORING_METRICS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_LIST_SRC_LIB_MONITORING_METRICS_H
#include "core.hpp"

class MetricsService;

class MetricsService : public object, public std::enable_shared_from_this<MetricsService> {
public:
    using std::enable_shared_from_this<MetricsService>::shared_from_this;
    static Record<string, double> metrics;

    static void recordApiLatency(string path, double latency);
    static void recordProjectView(string projectId, string userId = std::nullopt);
    static any getMetrics();
};

#endif
