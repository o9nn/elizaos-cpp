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

/**
 * Automated Reconciliation Cron Job
 *
 * Runs every 5 minutes to sync database state with blockchain state
 *
 * Set up in vercel.json or external cron service:
 * - Vercel: Add to vercel.json crons array
 * - AWS: Use EventBridge rule
 * - External: Use cron-job.org or similar
 */

const CRON_SECRET =
  process.env.CRON_SECRET || process.env.RECONCILIATION_SECRET;

async ,
      { status: 500 },
    );
  }

  if (CRON_SECRET && authHeader !== `Bearer ${CRON_SECRET}`) {
    console.warn("[Reconciliation Cron] Unauthorized access attempt", {
      ip:
        request.headers.get("x-forwarded-for") ||
        request.headers.get("x-real-ip"),
      timestamp: new Date().toISOString(),
    });
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  console.log("[Reconciliation Cron] Starting reconciliation task...");

  // Ensure runtime is initialized before running reconciliation
  const runtime = await agentRuntime.getRuntime();

  // Wait a moment for services to be registered after initialization
  // Services are registered during plugin initialization which happens in runtime.initialize()
  const maxRetries = 5;
  let retries = 0;
  let quoteService = runtime.getService("QuoteService");

  while (!quoteService && retries < maxRetries) {
    await new Promise((resolve) => setTimeout(resolve, 200));
    quoteService = runtime.getService("QuoteService");
    retries++;
  }

  if (!quoteService) {
    console.error(
      "[Reconciliation Cron] QuoteService not available after initialization",
    );
    return NextResponse.json(
      {
        error:
          "QuoteService not registered. Please check plugin configuration.",
      },
      { status: 503 },
    );
  }

  const startTime = Date.now();
  await runReconciliationTask();
  const duration = Date.now() - startTime;

  console.log(`[Reconciliation Cron] Completed in ${duration}ms`);

  return NextResponse.json({
    success: true,
    action: "reconcile_all",
    duration,
    timestamp: new Date().toISOString(),
  });
}

// Support POST for some cron services
async 

} // namespace elizaos
