#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"
#include "queries.hpp"

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
    double getChainId(SupportedNetwork = 'base' chain);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<MorphoService> start(IAgentRuntime runtime);
    std::future<void> stop();
    Promise<`0x$ supply(std::optional<std::any> params, const std::any& clients);
    ", assetsBase::toString());

      const hashes: `0x$ log(auto "Assets to supply (base units);
    void for(auto let i = 0; i < requests.length; i++);
    void catch(const std::any& err);
    Promise<`0x$ supplyCollateral(std::optional<std::any> params, const std::any& clients);
    ", assetsBase::toString());

      const hashes: `0x$ log(auto "Assets to supply (base units);
    void for(auto let i = 0; i < requests.length; i++);
    void catch(const std::any& err);
    Promise<`0x$ borrow(std::optional<std::any> params, const std::any& clients);
    ", assetsBase::toString());

      const hashes: `0x$ log(auto "Assets to borrow (base units);
    void for(auto let i = 0; i < requests.length; i++);
    void catch(const std::any& err);
    Promise<`0x$ repay(std::optional<std::any> params, const std::any& clients);
    ", assetsBase.toString());

      const hashes: `0x$ log(auto "Assets to repay (base units);
    void for(auto let i = 0; i < requests.length; i++);
    void catch(const std::any& err);
    Promise<`0x$ withdraw(std::optional<std::any> params, const std::any& clients);
    ", assetsBase.toString());

      const hashes: `0x$ log(auto "Assets to withdraw (base units);
    void for(auto let i = 0; i < requests.length; i++);
    void catch(const std::any& err);
    Promise<`0x$ withdrawCollateral(std::optional<std::any> params, const std::any& clients);
    ", assetsBase::toString());

      const hashes: `0x$ log(auto "Assets to withdraw (base units);
    void for(auto let i = 0; i < requests.length; i++);
    void catch(const std::any& err);
    Promise< buildSupplyTx(const std::any& params, PublicClient pc);
    void if(auto currentAllowance < assetsBase);
    Promise< buildSupplyCollateralTx(const std::any& params, PublicClient pc);
    void if(auto currentAllowance < assetsBase);
    Promise< buildBorrowTx(const std::any& params, PublicClient pc);
    Promise< buildRepayTx(std::optional<std::any> params, PublicClient pc);
    void if(auto params.fullRepayment);
    void if(auto params.shares);
    void if(auto params.assets);
    Promise< buildWithdrawTx(std::optional<std::any> params, PublicClient pc);
    void if(auto params.assets);
    void if(auto params.shares);
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
    void if(auto !response.ok);
    void catch(auto error);
    std::future<UserPosition> buildUserPosition(`0x${string}` address, const std::string& marketId, SupportedNetwork chain, PublicClient pc);
    void for(auto let i = 0; i < requests.length; i++);
    void catch(const std::any& err);

    void for(auto let i = 0; i < requests.length; i++);
    void catch(const std::any& err);
    void if(auto needsApproval);

// ----------------------------
// GraphQL Client
// ----------------------------
class GqlClient {
public:
    GqlClient(const std::string& private url);
    void if(auto !res.ok);
    void if(auto json.errors.length);
};

// --- Minimal ABIs ---

// Morpho Blue ABI (minimal)


} // namespace elizaos
