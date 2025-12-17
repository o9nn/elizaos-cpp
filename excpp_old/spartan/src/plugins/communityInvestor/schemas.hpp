#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Core schema definitions for community-trader plugin
 * This approach provides runtime validation and better type safety
 */

// Define consistent transaction types

// Define TokenPerformance schema

// Define Transaction schema - ensure consistency with types.ts
  // Use lowercase transaction types consistently
  // Use number for amounts consistently
  // Store timestamps as ISO strings consistently

// Define RecommenderMetrics schema

// Define Position schema
  // Store numeric amounts as strings to avoid precision issues

// Define TokenRecommendation schema

// Types derived from schemas
using TokenPerformance = z::infer<typeof tokenPerformanceSchema>;
using RecommenderMetrics = z::infer<typeof recommenderMetricsSchema>;
using Position = z::infer<typeof positionSchema>;
using TokenRecommendation = z::infer<typeof tokenRecommendationSchema>;

/**
 * Transform functions to convert database objects to schema-validated objects
 */

TokenPerformance transformTokenPerformance(const std::any& dbToken, auto chain = 'unknown');

Transaction transformTransaction(const std::any& dbTx, auto positionId = 'unknown', auto chain = 'unknown');

Position transformPosition(const std::any& dbPos);

/**
 * Type definition for MessageRecommendation based on the schema recommendationSchema
 */
using MessageRecommendation = z::infer<typeof recommendationSchema>;

/**
 * Schema for extracting trading recommendations from conversational text, capturing the key components of who made the recommendation, what asset was discussed, what action was recommended, and how strongly it was recommended
 */

TokenRecommendation transformTokenRecommendation(const std::any& dbRec);

} // namespace elizaos
