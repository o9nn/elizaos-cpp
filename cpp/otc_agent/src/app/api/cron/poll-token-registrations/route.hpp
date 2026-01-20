#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Store last processed state
// Note: In-memory state resets on each Vercel function invocation
// For production, consider using Vercel KV or database for persistence
// Current implementation checks last 1000 blocks each run (safe for 2-min intervals)

// Try to load from environment (set via Vercel KV or external service if needed)
std::optional<bigint> getLastBaseBlock();

/**
 * Poll for new token registrations (Base)
 */
std::future<void> pollBaseRegistrations();

/**
 * Poll for new token registrations (Solana)
 */
std::future<void> pollSolanaRegistrations();

/**
 * Vercel Cron Job Handler
 * Runs periodically to poll for new token registrations
 */
std::future<void> GET(NextRequest request);

} // namespace elizaos
