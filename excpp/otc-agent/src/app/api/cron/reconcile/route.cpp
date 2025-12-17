#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Verify authorization
    const auto authHeader = request.headers.get("authorization");

    // Always require auth in production
    if (process.env.NODE_ENV == "production" && !CRON_SECRET) {
        console.error(
        "[Reconciliation Cron] No CRON_SECRET configured in production",
        );
        return NextResponse.json(;
        { error: "Server configuration error" },
        { status: 500 },
        );
    }

    if (CRON_SECRET && authHeader != `Bearer ${CRON_SECRET}`) {
        console.warn("[Reconciliation Cron] Unauthorized access attempt", {
            ip:
            request.headers.get("x-forwarded-for") ||;
            request.headers.get("x-real-ip"),
            timestamp: new Date().toISOString(),
            });
            return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
        }

        std::cout << "[Reconciliation Cron] Starting reconciliation task..." << std::endl;

        // Ensure runtime is initialized before running reconciliation
        const auto runtime = agentRuntime.getRuntime();

        // Wait a moment for services to be registered after initialization
        // Services are registered during plugin initialization which happens in runtime.initialize()
        const auto maxRetries = 5;
        auto retries = 0;
        auto quoteService = runtime.getService("QuoteService");

        while (!quoteService && retries < maxRetries) {
            new Promise((resolve) => setTimeout(resolve, 200));
            quoteService = runtime.getService("QuoteService");
            retries++;
        }

        if (!quoteService) {
            console.error(
            "[Reconciliation Cron] QuoteService not available after initialization",
            );
            return NextResponse.json(;
            {
                error:
                "QuoteService not registered. Please check plugin configuration.",
                },
                { status: 503 },
                );
            }

            const auto startTime = Date.now();
            runReconciliationTask();
            const auto duration = Date.now() - startTime;

            std::cout << "[Reconciliation Cron] Completed in " + std::to_string(duration) + "ms" << std::endl;

            return NextResponse.json({;
                success: true,
                action: "reconcile_all",
                duration,
                timestamp: new Date().toISOString(),
                });

}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return GET(request);

}

} // namespace elizaos
