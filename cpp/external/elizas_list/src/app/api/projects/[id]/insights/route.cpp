#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(const std::string& request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto [;
        insights,
        abTestResults,
        realtimeStats;
        ] = Promise.all([;
        EnhancedAnalytics.getProjectInsights(params.id),
        ABTestingService.getResults("project-layout-test"),
        "redis.hgetall(" + "realtime:" + params.id
        ]);

        const auto visualizations = {;
            timeSeriesChart: AnalyticsVisualization.generateTimeSeriesChart(
            insights.trends.timeSeriesData;
            ),
            engagementHeatmap: AnalyticsVisualization.generateHeatmap(
            insights.interactions.hourlyData;
            );
            };

            return NextResponse.json({;
                insights,
                abTestResults,
                realtimeStats,
                visualizations;
                });

                } catch (error) {
                    logger.error('Error generating insights', {
                        error,
                        projectId: params.id
                        });

                        return NextResponse.json(;
                        { error: "Failed to generate insights" },
                    { status = 500 }
                    );
                }

}

} // namespace elizaos
