#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;

async : { params: { id: string } }
) {
  const startTime = Date.now();
  
  try {
    const insights = await EnhancedAnalytics.getProjectInsights(params.id);
    
    // Record API latency
    const latency = Date.now() - startTime;
    await MetricsService.recordApiLatency('/api/projects/analytics', latency);

    return NextResponse.json(insights);
  } catch (error) {
    logger.error('Error fetching project analytics', {
      error,
      projectId: params.id
    });

    return NextResponse.json(
      { error: 'Failed to fetch analytics' },
      { status: 500 }
    );
  }
} 
} // namespace elizaos
