#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_MONITORING_REAL-TIME-MONITOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_MONITORING_REAL-TIME-MONITOR_H
#include "core.h"
#include "ioredis.h"
#include "../ml/anomaly-detection.h"
#include "./logger.h"
#include "./metrics.h"

class RealTimeMonitor;

class RealTimeMonitor : public object, public std::enable_shared_from_this<RealTimeMonitor> {
public:
    using std::enable_shared_from_this<RealTimeMonitor>::shared_from_this;
    std::shared_ptr<Redis> redis;

    std::shared_ptr<AnomalyDetector> anomalyDetector;

    Record<string, double> alertThresholds;

    RealTimeMonitor();
    virtual void startMonitoring();
    virtual void monitorMetrics();
    virtual void handleAnomalies(array<std::shared_ptr<AnomalyResult>> anomalies);
    virtual void triggerAlerts(array<std::shared_ptr<AnomalyResult>> anomalies);
};

#endif
