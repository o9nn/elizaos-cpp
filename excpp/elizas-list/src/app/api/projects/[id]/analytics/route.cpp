#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(Request request, const std::any& { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto startTime = Date.now();

    try {
        const auto insights = EnhancedAnalytics.getProjectInsights(params.id);

        // Record API latency
        const auto latency = Date.now() - startTime;
        MetricsService.recordApiLatency('/api/projects/analytics', latency);

        return NextResponse.json(insights);
        } catch (error) {
            logger.error('Error fetching project analytics', {
                error,
                projectId: params.id
                });

                return NextResponse.json(;
                { error: 'Failed to fetch analytics' },
            { status = 500 }
            );
        }

}

} // namespace elizaos
