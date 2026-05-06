#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/monitoring/real-time-monitor.h"

RealTimeMonitor::RealTimeMonitor() {
    this->redis = std::make_shared<Redis>(process->env->REDIS_URL);
    this->anomalyDetector = std::make_shared<AnomalyDetector>();
    this->alertThresholds = object{
        object::pair{std::string("errorRate"), 0.05}, 
        object::pair{std::string("latency"), 1000}, 
        object::pair{std::string("userEngagement"), 0.4}
    };
}

void RealTimeMonitor::startMonitoring()
{
    std::async([=]() { this->anomalyDetector->initialize(); });
    this->monitorMetrics();
    this->monitorErrors();
    this->monitorUserBehavior();
}

void RealTimeMonitor::monitorMetrics()
{
    auto metrics = std::async([=]() { MetricsService::getCurrentMetrics(); });
    auto anomalies = std::async([=]() { this->anomalyDetector->detectAnomalies(this->prepareMetricsData(metrics)); });
    if (anomalies->some([=](auto a) mutable
    {
        return a->isAnomaly;
    }
    )) {
        std::async([=]() { this->handleAnomalies(anomalies); });
    }
}

void RealTimeMonitor::handleAnomalies(array<std::shared_ptr<AnomalyResult>> anomalies)
{
    auto criticalAnomalies = anomalies->filter([=](auto a) mutable
    {
        return a->score > this->alertThresholds->errorRate * 2;
    }
    );
    if (criticalAnomalies->get_length() > 0) {
        std::async([=]() { this->triggerAlerts(criticalAnomalies); });
    }
    std::async([=]() { this->storeAnomalyData(anomalies); });
}

void RealTimeMonitor::triggerAlerts(array<std::shared_ptr<AnomalyResult>> anomalies)
{
    auto alerts = anomalies->map([=](auto anomaly) mutable
    {
        return (object{
            object::pair{std::string("severity"), this->calculateSeverity(anomaly)}, 
            object::pair{std::string("message"), this->formatAlertMessage(anomaly)}, 
            object::pair{std::string("timestamp"), std::make_shared<Date>()}
        });
    }
    );
    std::async([=]() { Promise->all(std::tuple<any, any, any>{ this->sendSlackAlerts(alerts), this->sendEmailAlerts(alerts), this->triggerPagerDuty(alerts->filter([=](auto a) mutable
    {
        return a["severity"] == std::string("critical");
    }
    )) }); });
}

