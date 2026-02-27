#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/app/api/analytics/insights/route.h"

any GET(std::shared_ptr<Request> request)
{
    try
    {
        auto searchParams = ((std::make_shared<URL>(request->url)))->searchParams;
        auto projectId = searchParams->get(std::string("projectId"));
        auto segments = std::async([=]() { UserSegmentation->generateSegments(); });
        auto timeSeriesData = array<std::shared_ptr<TimeSeriesDataPoint>>();
        auto interactionData = object{};
        auto visualizations = object{
            object::pair{std::string("timeSeriesChart"), AnalyticsVisualization->generateTimeSeriesChart(timeSeriesData)}, 
            object::pair{std::string("engagementChart"), AnalyticsVisualization->generateHeatmap(interactionData)}
        };
        return NextResponse->json(object{
            object::pair{std::string("segments"), std::string("segments")}, 
            object::pair{std::string("visualizations"), std::string("visualizations")}
        });
    }
    catch (const any& error)
    {
        logger->error(std::string("Error generating insights:"), error);
        return NextResponse->json(object{
            object::pair{std::string("error"), std::string("Failed to generate insights")}
        }, object{
            object::pair{std::string("status"), 500}
        });
    }
};


