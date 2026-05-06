#include "..bus.hpp"
#include "..types/jobs.hpp"
#include "elizaos/core.hpp"
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



// Import Coinbase facilitator for mainnet

// In-memory job storage

// Track cleanup interval

/**
 * Cleanup expired jobs
 */
void cleanupExpiredJobs();

/**
 * Initialize cleanup interval
 */
void startCleanupInterval();

/**
 * Stop cleanup interval
 */
void stopCleanupInterval();

/**
 * Convert Job to JobDetailsResponse
 */
JobDetailsResponse jobToResponse(Job job);

/**
 * Validate CreateJobRequest
 * Note: userId is optional - x402 middleware handles payment validation
 */

/**
 * Extended Router interface with cleanup method
 */

/**
 * Creates the jobs router for one-off messaging with x402 payment support
 * 
 * This endpoint requires x402 payment ($0.015 per request) on Base mainnet.
 * Payment is handled via Coinbase facilitator, which verifies and settles payments automatically.
 * 
 * Configuration:
 * - Price: $0.015 USDC per request
 * - Network: Base mainnet
 * - Default timeout: 3 minutes (180000ms)
 * - Maximum timeout: 5 minutes (300000ms)
 * 
 * Capabilities:
 * - Research: Query and analyze research data, papers, and academic resources
 * - News: Fetch and summarize current news articles from various sources
 * - Information Processing: Process and synthesize information from multiple sources
 * - Data Analysis: Analyze trends, patterns, and insights from provided data
 * 
 * Note: This endpoint does not support swap operations or direct EVM transaction capabilities.
 * Focus is on research, news, and information processing tasks.
 */
JobsRouter createJobsRouter(ElizaOS elizaOS, AgentServer serverInstance);


} // namespace elizaos
