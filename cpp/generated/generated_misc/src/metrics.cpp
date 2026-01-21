#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/monitoring/metrics.h"

Record<std::string, double> MetricsService::metrics = object{};

void MetricsService::recordApiLatency(std::string path, double latency)
{
    auto key = std::string("api_latency_") + path + string_empty;
    MetricsService::metrics[key] = (OR((const_(MetricsService::metrics)[key]), (0))) + latency;
}

void MetricsService::recordProjectView(std::string projectId, std::string userId)
{
    auto key = std::string("project_views_") + projectId + string_empty;
    MetricsService::metrics[key] = (OR((const_(MetricsService::metrics)[key]), (0))) + 1;
}

std::any MetricsService::getMetrics()
{
    return MetricsService::metrics;
}

