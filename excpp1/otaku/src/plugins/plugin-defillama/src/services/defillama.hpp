#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

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

class DefiLlamaService {
public:
    DefiLlamaService(IAgentRuntime runtime);
    std::future<DefiLlamaService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<std::future<std::vector<ProtocolLookupResult>>> getProtocolsByNames(const std::vector<std::string>& names);
    std::future<std::vector<ProtocolSummary>> searchProtocolCandidates(const std::string& query, number = 5 maxResults);
    std::future<std::vector<YieldPool>> searchYields(std::optional<std::any> params);
    std::future<std::vector<YieldChartPoint>> getPoolChart(const std::string& poolId);
    std::future<ProtocolTvlHistory> getProtocolTvlHistory(const std::string& slug);
    std::future<std::vector<ChainTvlPoint>> getChainTvlHistory(const std::string& chain, std::optional<ChainTvlHistoryOptions> options);
    std::future<void> ensureFresh();
    std::future<void> ensureYieldsFresh();
    void for(auto let attempt = 1; attempt <= maxAttempts; attempt++);
    std::future<void> loadIndex();
    std::future<void> loadYieldsPools();
    void setProtocolHistoryCache(const std::string& key, ProtocolTvlHistory data, double now);
    void setChainHistoryCache(const std::string& key, const std::vector<ChainTvlPoint>& data, double now);
    void evictExpiredHistoryCaches(double now);
    void rebuildProtocolIndex();
};

using RawProtocolHistoryPoint = {

using RawChainHistoryEntry = std::vector<{

using RawProtocolHistory = {

using RawChainTvlPoint = {

std::optional<YieldPool> shapeYieldPool(RawYieldPool raw);

std::optional<double> normalizeNullableNumber(const std::variant<double, std::string>& value);

ProtocolSummary shapeProtocol(DefiLlamaProtocol p);

std::unordered_map<std::string, double> shapeChainTvlsRecord(ChainTvlsRawRecord value);

ProtocolTvlHistory shapeProtocolHistory(RawProtocolHistory raw, const std::string& fallbackSlug);


} // namespace elizaos
