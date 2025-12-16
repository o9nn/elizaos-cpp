#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



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

std::future<void> GET(NextRequest request);

// Support POST for some cron services
std::future<void> POST(NextRequest request);

} // namespace elizaos
