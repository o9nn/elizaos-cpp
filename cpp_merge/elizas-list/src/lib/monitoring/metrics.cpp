#include "metrics.hpp"

Record<string, double> MetricsService::metrics = object{};

void MetricsService::recordApiLatency(string path, double latency)
{
    auto key = std:("api_latency_") + path + string_empty;
    MetricsService::metrics[key] = (OR((const_(MetricsService::metrics)[key]), (0))) + latency;
}

void MetricsService::recordProjectView(string projectId, string userId)
{
    auto key = std:("project_views_") + projectId + string_empty;
    MetricsService::metrics[key] = (OR((const_(MetricsService::metrics)[key]), (0))) + 1;
}

any MetricsService::getMetrics()
{
    return MetricsService::metrics;
}

