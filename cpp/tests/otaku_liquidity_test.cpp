#include <gtest/gtest.h>

#include "elizaos/otaku.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

namespace {

const std::string kWallet = "0x1111111111111111111111111111111111111111";
const std::string kOther = "0x2222222222222222222222222222222222222222";
const std::string kPool = "0x3333333333333333333333333333333333333333";
const std::string kNft = "0x4444444444444444444444444444444444444444";

template <typename T>
OperationResult<T> ok(T value, const std::string& evidence = "fake-evidence") {
    OperationResult<T> result;
    result.code = ResultCode::OK;
    result.value = std::move(value);
    result.evidence = evidence;
    return result;
}

class PositionMarket final : public MarketDataAdapter {
public:
    OperationResult<double> getPoolApr(const std::string&, DexProtocol) override {
        return ok(7.25, "fixture:pool-snapshot");
    }

    OperationResult<std::vector<YieldPosition>> findBestYields(
        const std::string& asset, YieldStrategy strategy, double) override {
        YieldPosition lower;
        lower.protocol = "lower";
        lower.asset = asset;
        lower.apy = 2.0;
        lower.strategy = strategy;
        YieldPosition higher = lower;
        higher.protocol = "higher";
        higher.apy = 4.0;
        return ok(std::vector<YieldPosition>{lower, higher});
    }

    OperationResult<std::vector<std::string>> getSupportedProtocols(ChainId) override {
        return ok(std::vector<std::string>{"zeta", "alpha", "alpha"});
    }

    OperationResult<std::vector<NFTInfo>> getOwnedNFTs(
        const std::string& wallet, ChainId chain) override {
        NFTInfo first;
        first.contractAddress = kNft;
        first.tokenId = "2";
        first.owner = wallet;
        first.chainId = chain;
        NFTInfo second = first;
        second.tokenId = "1";
        return ok(std::vector<NFTInfo>{first, second});
    }

    OperationResult<NFTInfo> getNFTDetails(
        const std::string& contract, const std::string& tokenId, ChainId chain) override {
        NFTInfo nft;
        nft.contractAddress = contract;
        nft.tokenId = tokenId;
        nft.name = "fixture NFT";
        nft.owner = kWallet;
        nft.chainId = chain;
        return ok(nft);
    }

    OperationResult<double> getNFTFloorPrice(const std::string&, ChainId) override {
        return ok(1.5, "fixture:floor");
    }
};

class PositionExecution final : public BlockchainExecutionAdapter {
public:
    TxStatus status = TxStatus::SIMULATED;
    bool fail = false;
    std::atomic<unsigned> sequence{0};

    OperationResult<BlockchainExecutionResult> execute(
        const BlockchainExecutionRequest& request) override {
        if (fail) {
            OperationResult<BlockchainExecutionResult> result;
            result.code = ResultCode::ADAPTER_ERROR;
            result.message = "position adapter failed";
            return result;
        }
        BlockchainExecutionResult result;
        result.receipt.txHash = "local-position-" + std::to_string(sequence.fetch_add(1));
        result.receipt.status = status;
        result.receipt.chainId = request.chainId;
        result.receipt.from = request.from;
        result.receipt.to = request.to;
        result.receipt.timestamp = std::chrono::system_clock::time_point{100s};
        result.receipt.evidence = "fixture:receipt";
        result.receipt.evidenceVerified = true;
        if (status == TxStatus::CONFIRMED) result.receipt.blockNumber = 50;

        if (request.operation == BlockchainOperation::ADD_LIQUIDITY) {
            LiquidityPosition position;
            position.positionId = "lp-1";
            position.poolAddress = kPool;
            position.token0 = request.asset;
            position.token1 = request.secondaryAsset;
            position.amount0 = request.amount;
            position.amount1 = request.secondaryAmount;
            position.liquidityTokens = 10.0;
            position.currentValue = 100.0;
            position.chainId = request.chainId;
            position.protocol = DexProtocol::UNISWAP_V3;
            position.owner = request.from;
            position.receiptTxHash = result.receipt.txHash;
            position.status = status;
            position.evidence = "fixture:lp-position";
            result.liquidityPosition = position;
        }
        if (request.operation == BlockchainOperation::NFT_BUY) {
            NFTInfo nft;
            nft.contractAddress = request.asset;
            nft.tokenId = request.referenceId;
            nft.owner = request.from;
            nft.chainId = request.chainId;
            nft.receiptTxHash = result.receipt.txHash;
            nft.status = status;
            nft.evidence = "fixture:nft-purchase";
            result.nft = nft;
        }
        if (request.operation == BlockchainOperation::YIELD_DEPOSIT) {
            YieldPosition position;
            position.positionId = "yield-1";
            position.protocol = request.metadata.at("protocol");
            position.asset = request.asset;
            position.depositedAmount = request.amount;
            position.currentValue = request.amount;
            position.earnedRewards = 2.0;
            position.chainId = request.chainId;
            position.depositedAt = std::chrono::system_clock::time_point{50s};
            position.owner = request.from;
            position.receiptTxHash = result.receipt.txHash;
            position.status = status;
            position.evidence = "fixture:yield-position";
            result.yieldPosition = position;
        }
        return ok(result);
    }
};

TEST(OtakuLiquidity, AdapterEvidenceControlsBookkeeping) {
    auto market = std::make_shared<PositionMarket>();
    auto execution = std::make_shared<PositionExecution>();
    OtakuAgent agent("liquidity", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    auto added = agent.addLiquidityResult(
        "ETH", "USDC", 1.0, 10.0, DexProtocol::UNISWAP_V3);
    ASSERT_TRUE(added);
    EXPECT_EQ(added.value.status, TxStatus::SIMULATED);
    ASSERT_EQ(agent.getLiquidityPositions().size(), 1U);
    EXPECT_EQ(agent.getLiquidityPositions()[0].owner, kWallet);
    EXPECT_DOUBLE_EQ(agent.getPoolApr(kPool, DexProtocol::UNISWAP_V3), 7.25);

    auto partial = agent.removeLiquidityResult("lp-1", 25.0);
    ASSERT_TRUE(partial);
    ASSERT_EQ(agent.getLiquidityPositions().size(), 1U);
    EXPECT_DOUBLE_EQ(agent.getLiquidityPositions()[0].amount0, 0.75);
    EXPECT_DOUBLE_EQ(agent.getLiquidityPositions()[0].amount1, 7.5);

    EXPECT_EQ(agent.removeLiquidityResult("lp-1", 0.0).code,
              ResultCode::INVALID_ARGUMENT);
    EXPECT_EQ(agent.removeLiquidityResult("unknown", 50.0).code,
              ResultCode::NOT_FOUND);
    EXPECT_TRUE(agent.removeLiquidity("lp-1"));
    EXPECT_TRUE(agent.getLiquidityPositions().empty());
}

TEST(OtakuLiquidity, AdapterFailureDoesNotCreateOrMutatePositions) {
    auto market = std::make_shared<PositionMarket>();
    auto execution = std::make_shared<PositionExecution>();
    execution->fail = true;
    OtakuAgent agent("liquidity-failure", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    EXPECT_EQ(agent.addLiquidityResult(
                  "ETH", "USDC", 1.0, 10.0, DexProtocol::UNISWAP_V3).code,
              ResultCode::ADAPTER_ERROR);
    EXPECT_TRUE(agent.getLiquidityPositions().empty());
}

TEST(OtakuYield, DepositWithdrawHarvestAndOwnershipAreBookkept) {
    auto market = std::make_shared<PositionMarket>();
    auto execution = std::make_shared<PositionExecution>();
    OtakuAgent agent("yield", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    auto deposited = agent.depositToYieldResult("aave", "USDC", 10.0);
    ASSERT_TRUE(deposited);
    ASSERT_EQ(agent.getYieldPositions().size(), 1U);
    EXPECT_EQ(agent.getYieldPositions()[0].positionId, "yield-1");
    EXPECT_DOUBLE_EQ(agent.getYieldPositions()[0].depositedAmount, 10.0);

    auto withdrawn = agent.withdrawFromYieldResult("yield-1", 4.0);
    ASSERT_TRUE(withdrawn);
    ASSERT_EQ(agent.getYieldPositions().size(), 1U);
    EXPECT_DOUBLE_EQ(agent.getYieldPositions()[0].depositedAmount, 6.0);
    EXPECT_EQ(agent.withdrawFromYieldResult("yield-1", 7.0).code,
              ResultCode::INVALID_ARGUMENT);
    EXPECT_EQ(agent.withdrawFromYieldResult("not-owned", 1.0).code,
              ResultCode::NOT_FOUND);

    auto harvested = agent.harvestAllRewardsResult();
    ASSERT_TRUE(harvested);
    EXPECT_DOUBLE_EQ(agent.getYieldPositions()[0].earnedRewards, 0.0);
}

TEST(OtakuYield, DiscoveryIsAdapterBackedAndDeterministic) {
    auto market = std::make_shared<PositionMarket>();
    YieldManager manager(market, {});
    auto yields = manager.findBestYields("USDC", YieldStrategy::LENDING);
    ASSERT_EQ(yields.size(), 2U);
    EXPECT_EQ(yields[0].protocol, "higher");
    EXPECT_EQ(manager.getSupportedProtocols(ChainId::ETHEREUM_MAINNET),
              (std::vector<std::string>{"alpha", "zeta"}));
    YieldManager unsupported;
    EXPECT_TRUE(unsupported.findBestYields("USDC", YieldStrategy::LENDING).empty());
}

TEST(OtakuNft, OwnershipQueriesAndOperationsRequireAdapters) {
    auto market = std::make_shared<PositionMarket>();
    auto execution = std::make_shared<PositionExecution>();
    OtakuAgent agent("nft", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    auto owned = agent.getOwnedNFTs();
    ASSERT_EQ(owned.size(), 2U);
    EXPECT_EQ(owned[0].tokenId, "1");
    EXPECT_TRUE(agent.transferNFT(kNft, "1", kOther));
    EXPECT_TRUE(agent.buyNFT(kNft, "1", 2.0));
    EXPECT_FALSE(agent.transferNFT("invalid", "1", kOther));
    EXPECT_FALSE(agent.buyNFT(kNft, "1", -1.0));

    OtakuAgent unsupported("nft-default");
    ASSERT_TRUE(unsupported.connectWallet(kWallet));
    EXPECT_TRUE(unsupported.getOwnedNFTs().empty());
    EXPECT_FALSE(unsupported.transferNFT(kNft, "1", kOther));
}

TEST(OtakuOperations, BridgeTransferAndBatchUseExecutionReceipts) {
    auto market = std::make_shared<PositionMarket>();
    auto execution = std::make_shared<PositionExecution>();
    OtakuAgent agent("operations", market, execution);
    ASSERT_TRUE(agent.connectWallet(kWallet));
    EXPECT_EQ(agent.transferToken(kOther, "ETH", 1.0).status, TxStatus::SIMULATED);
    EXPECT_EQ(agent.batchTransfer({{kOther, 1.0}}, "ETH").status, TxStatus::SIMULATED);

    BridgeQuote bridge;
    bridge.sourceChain = ChainId::ETHEREUM_MAINNET;
    bridge.destChain = ChainId::BASE;
    bridge.token = "ETH";
    bridge.inputAmount = 1.0;
    bridge.outputAmount = 0.99;
    bridge.bridgeFee = 0.01;
    bridge.estimatedTime = 10.0;
    bridge.protocol = BridgeProtocol::ACROSS;
    bridge.validUntil = std::chrono::system_clock::now() + 60s;
    bridge.quoteId = "fixture-bridge";
    bridge.evidence = "fixture:bridge";
    bridge.evidenceVerified = true;
    EXPECT_EQ(agent.executeBridgeWithQuote(bridge).status, TxStatus::SIMULATED);
}

} // namespace
