#include "route.hpp"

any GET(std::shared_ptr<Request> request)
{
    try
    {
        auto searchParams = ((std::make_shared<URL>(request->url)))->searchParams;
        auto projectId = searchParams->get(std:("projectId"));
        auto segments = std::async([=]() { UserSegmentation->generateSegments(); });
        auto timeSeriesData = array<std::shared_ptr<TimeSeriesDataPoint>>();
        auto interactionData = object{};
        auto visualizations = object{
            object::pair{std:("timeSeriesChart"), AnalyticsVisualization->generateTimeSeriesChart(timeSeriesData)}, 
            object::pair{std:("engagementChart"), AnalyticsVisualization->generateHeatmap(interactionData)}
        };
        return NextResponse->json(object{
            object::pair{std:("segments"), std:("segments")}, 
            object::pair{std:("visualizations"), std:("visualizations")}
        });
    }
    catch (const any& error)
    {
        logger->error(std:("Error generating insights:"), error);
        return NextResponse->json(object{
            object::pair{std:("error"), std:("Failed to generate insights")}
        }, object{
            object::pair{std:("status"), 500}
        });
    }
};


