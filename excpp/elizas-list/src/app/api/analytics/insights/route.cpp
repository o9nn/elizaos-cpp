#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(Request request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto searchParams = new URL(request.url).searchParams;
        const auto projectId = searchParams.get('projectId');

        const auto segments = UserSegmentation.generateSegments();
        const std::vector<TimeSeriesDataPoint> timeSeriesData = [];
        const InteractionData interactionData = {};

        const auto visualizations = {;
            timeSeriesChart: AnalyticsVisualization.generateTimeSeriesChart(timeSeriesData),
            engagementChart: AnalyticsVisualization.generateHeatmap(interactionData)
            };

            return NextResponse.json({ segments, visualizations });
            } catch (error) {
                std::cerr << 'Error generating insights:' << error << std::endl;
                return NextResponse.json({ error: 'Failed to generate insights' }, { status: 500 });
            }

}

} // namespace elizaos
