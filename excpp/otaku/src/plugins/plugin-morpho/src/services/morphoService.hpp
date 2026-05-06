#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "queries.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// ----------------------------
// Constants
// ----------------------------

// Chain ID mapping for supported networks

class MorphoService {
public:
    MorphoService(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    static std::future<MorphoService> start(IAgentRuntime runtime);
    std::future<void> stop();
    Promise<`0x$ supply(std::optional<std::any> params, const std::any& clients);
    Promise<`0x$ supplyCollateral(std::optional<std::any> params, const std::any& clients);
    Promise<`0x$ borrow(std::optional<std::any> params, const std::any& clients);
    Promise<`0x$ repay(std::optional<std::any> params, const std::any& clients);
    Promise<`0x$ withdraw(std::optional<std::any> params, const std::any& clients);
    Promise<`0x$ withdrawCollateral(std::optional<std::any> params, const std::any& clients);
    Promise< buildSupplyTx(const std::any& params, PublicClient pc);
    Promise< buildSupplyCollateralTx(const std::any& params, PublicClient pc);
    Promise< buildBorrowTx(const std::any& params, PublicClient pc);
    Promise< buildRepayTx(std::optional<std::any> params, PublicClient pc);
    Promise< buildWithdrawTx(std::optional<std::any> params, PublicClient pc);
    Promise< buildWithdrawCollateralTx(const std::any& params, PublicClient pc);
    std::future<std::string> getMarketId(const std::string& input, SupportedNetwork chain);
    std::future<std::string> resolveMarketIdFromPair(const std::string& pair, SupportedNetwork chain);
    std::future<std::vector<std::any>> fetchMarketsFromApi(double chainId);
    std::future<std::vector<std::any>> fetchVaultsFromApi(double chainId);
    std::future<std::vector<std::string>> fetchPositionsFromApi(`0x${string}` address, double chainId);
    MarketSummary mapMarketSummary(const std::any& m);
    MorphoVaultData mapVault(const std::any& v);
    void mapVaultAllocations(MorphoVaultData out, const std::any& v);
    std::future<MarketSummary> getMarketSummary(const std::string& market, SupportedNetwork chain);
    std::future<MarketSummary> fetchMarketSummaryById(const std::string& uniqueKey, SupportedNetwork chain);
    std::future<UserPosition> buildUserPosition(`0x${string}` address, const std::string& marketId, SupportedNetwork chain, PublicClient pc);

// ----------------------------
// GraphQL Client
// ----------------------------
class GqlClient {
public:
    GqlClient();
};

// --- Minimal ABIs ---

// Morpho Blue ABI (minimal)


} // namespace elizaos
