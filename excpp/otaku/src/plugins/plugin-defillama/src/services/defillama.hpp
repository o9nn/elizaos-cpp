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



using ChainTvlsRawRecord = std::variant<Record<string, number, std::string, nullptr, { tvl?: number }>>;

using DefiLlamaProtocol = {

using YieldPool = {

using YieldChartPoint = {

using RawYieldPool = {

using ProtocolTvlPoint = {

using ProtocolTvlHistory = {

using ChainTvlPoint = {

using ChainTvlHistoryOptions = {

using ProtocolSummary = {

using ProtocolLookupResult = {

class DefiLlamaService extends Service {
  static serviceType = "defillama_protocols" as const;
  capabilityDescription = "Look up DeFiLlama protocols by name/symbol and yield opportunities (TTL-cached)";

  // Protocol TVL cache
  private cache: DefiLlamaProtocol[] = [];
  private cacheTimestampMs: number = 0;
  private ttlMs: number = 300000; // 5 minutes
  private protocolIndex: Map<string, DefiLlamaProtocol> = new Map();

  // Protocol history cache
  private protocolHistoryCache: Map<string, { timestamp: number; data: ProtocolTvlHistory }> = new Map();

  // Chain history cache

  // Yields cache

    // Initialize protocols TTL

    // Initialize yields TTL

    // Load both caches in parallel

  /**
   * Search for multiple protocol candidates matching a query (0-5 matches)
   * Returns protocols sorted by relevance and TVL
   */

      // Exact matches get highest priority
      // Starts with query
      // Contains query

        // Boost score by TVL (protocols with higher TVL ranked higher among same match type)

    // Sort by score descending

    // Take top N results

  /**
   * Search for yield opportunities by protocol, token, and/or chain
   */

    // Filter by protocol (fuzzy match)

    // Filter by token symbol (exact match, case-insensitive)

    // Filter by chain (case-insensitive)

    // Filter by minimum APY

    // Filter stablecoins only

    // Sort by APY descending (highest yields first)

    // Apply limit (default to top 10)

  /**
   * Get historical yield chart data for a specific pool
   */

using RawProtocolHistoryPoint = {

using RawChainHistoryEntry = std::vector<{

using RawProtocolHistory = {

using RawChainTvlPoint = {

ProtocolSummary shapeProtocol(DefiLlamaProtocol p);

std::unordered_map<std::string, double> shapeChainTvlsRecord(ChainTvlsRawRecord | undefined value);

ProtocolTvlHistory shapeProtocolHistory(RawProtocolHistory raw, const std::string& fallbackSlug);


} // namespace elizaos
