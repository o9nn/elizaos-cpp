#include <gtest/gtest.h>

#include "elizaos/spartan.hpp"

#include <atomic>
#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace elizaos {
namespace {

constexpr const char* kSolAddress = "So11111111111111111111111111111111111111112";
constexpr const char* kUsdcAddress = "EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v";
constexpr const char* kWallet = "11111111111111111111111111111111";
constexpr std::int64_t kObservedAt = 1700000000;

AdapterEvidence verifiedEvidence(const std::string& reference) {
    return {"fake-adapter", reference, kObservedAt, true};
}

TokenInfo solToken() {
    TokenInfo token("SOL", kSolAddress);
    token.price = 20.5;
    token.marketCap = 1.0;
    token.volume24h = 2.0;
    token.change24h = -0.25;
    return token;
}

TokenInfo usdcToken() {
    TokenInfo token("USDC", kUsdcAddress);
    token.price = 1.0;
    token.marketCap = 3.0;
    token.volume24h = 4.0;
    token.change24h = 0.0;
    return token;
}

class FakeAdapter final : public ISpartanAdapter {
public:
    ConnectionResult connection{OperationStatus::Verified, "connected", verifiedEvidence("connection-1")};
    TokenQueryResult tokenResult;
    TokenQueryResult topResult;
    TradeResult swapResult;
    TradeResult liquidityResult;
    OperationResult copyResult;
    bool throwOnToken = false;
    bool throwOnSwap = false;
    std::atomic<int> connectCalls{0};
    std::atomic<int> disconnectCalls{0};
    std::atomic<int> swapCalls{0};
    std::atomic<int> liquidityCalls{0};
    std::atomic<int> copyCalls{0};
    SpartanConfig seenConfig;
    SwapRequest seenSwap;
    LiquidityRequest seenLiquidity;
    CopyTradingRequest seenCopy;
    std::mutex mutex;

    FakeAdapter() {
        tokenResult.status = OperationStatus::Verified;
        tokenResult.message = "snapshot";
        tokenResult.token = solToken();
        tokenResult.evidence = verifiedEvidence("token-1");
        topResult.status = OperationStatus::Verified;
        topResult.message = "ranked snapshots";
        topResult.tokens = {solToken(), usdcToken()};
        topResult.evidence = verifiedEvidence("top-1");
        swapResult.status = OperationStatus::Verified;
        swapResult.transactionId = "verified-swap-signature";
        swapResult.message = "receipt finalized";
        swapResult.amountTraded = 5.0;
        swapResult.executionPrice = 20.0;
        swapResult.evidence = verifiedEvidence("swap-1");
        liquidityResult.status = OperationStatus::Verified;
        liquidityResult.transactionId = "verified-liquidity-signature";
        liquidityResult.message = "liquidity receipt finalized";
        liquidityResult.amountTraded = 10.0;
        liquidityResult.executionPrice = 1.0;
        liquidityResult.evidence = verifiedEvidence("liquidity-1");
        copyResult.status = OperationStatus::Verified;
        copyResult.message = "copy configuration finalized";
        copyResult.evidence = verifiedEvidence("copy-1");
    }

    ConnectionResult connect(const SpartanConfig& config) override {
        ++connectCalls;
        std::lock_guard<std::mutex> lock(mutex);
        seenConfig = config;
        return connection;
    }
    void disconnect() noexcept override { ++disconnectCalls; }
    TokenQueryResult queryTokenBySymbol(const std::string& symbol) override {
        if (throwOnToken) throw std::runtime_error("fake token failure");
        TokenQueryResult result = tokenResult;
        if (symbol == "USDC") result.token = usdcToken();
        return result;
    }
    TokenQueryResult queryTokenByAddress(const std::string& address) override {
        if (throwOnToken) throw std::runtime_error("fake token failure");
        TokenQueryResult result = tokenResult;
        result.token = address == kUsdcAddress ? usdcToken() : solToken();
        return result;
    }
    TokenQueryResult queryTopTokens(std::size_t count) override {
        TokenQueryResult result = topResult;
        if (result.tokens.size() > count) result.tokens.resize(count);
        return result;
    }
    TradeResult executeSwap(const SwapRequest& request) override {
        ++swapCalls;
        if (throwOnSwap) throw std::runtime_error("fake swap failure");
        std::lock_guard<std::mutex> lock(mutex);
        seenSwap = request;
        return swapResult;
    }
    TradeResult changeLiquidity(const LiquidityRequest& request) override {
        ++liquidityCalls;
        std::lock_guard<std::mutex> lock(mutex);
        seenLiquidity = request;
        TradeResult result = liquidityResult;
        result.amountTraded = request.amountA + request.amountB;
        return result;
    }
    OperationResult configureCopyTrading(const CopyTradingRequest& request) override {
        ++copyCalls;
        std::lock_guard<std::mutex> lock(mutex);
        seenCopy = request;
        return copyResult;
    }
};

SpartanConfig adapterConfig() {
    SpartanConfig config = getDefaultSpartanConfig();
    config.solanaRpcUrl = "https://example.invalid";
    config.solanaPublicKey = kWallet;
    config.solanaPrivateKey = "must-not-cross-boundary";
    config.maxTradeAmount = 100.0;
    config.maxRiskPercentage = 0.1;
    config.portfolioValue = 1000.0;
    return config;
}

class SpartanAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        adapter = std::make_shared<FakeAdapter>();
        agent = createSpartanAgent(adapterConfig(), adapter);
    }
    std::shared_ptr<FakeAdapter> adapter;
    std::unique_ptr<SpartanAgent> agent;
};

TEST_F(SpartanAdapterTest, ConnectionRejectionKeepsLocalReadyAndBlocksActions) {
    adapter->connection = {OperationStatus::Rejected, "RPC refused", {}};
    EXPECT_TRUE(agent->initialize());
    EXPECT_TRUE(agent->isInitialized());
    EXPECT_FALSE(agent->isExternallyConnected());
    EXPECT_EQ(agent->getConnectionStatus().status, OperationStatus::Rejected);
    EXPECT_EQ(agent->queryTokenInfo("SOL").status, OperationStatus::Rejected);
    EXPECT_EQ(agent->executeSwap("SOL", "USDC", 1.0, "Orca", true).status, OperationStatus::Rejected);
    EXPECT_EQ(adapter->swapCalls.load(), 0);
}

TEST_F(SpartanAdapterTest, ConnectionRequiresEvidenceAndNeverForwardsPrivateKey) {
    adapter->connection.evidence = {};
    EXPECT_TRUE(agent->initialize());
    EXPECT_FALSE(agent->isExternallyConnected());
    EXPECT_EQ(agent->getConnectionStatus().status, OperationStatus::Failed);
    std::lock_guard<std::mutex> lock(adapter->mutex);
    EXPECT_TRUE(adapter->seenConfig.solanaPrivateKey.empty());
}

TEST_F(SpartanAdapterTest, RetrievesOnlyVerifiedEvidenceBearingTokens) {
    ASSERT_TRUE(agent->initialize());
    ASSERT_TRUE(agent->isExternallyConnected());
    const auto result = agent->queryTokenInfo("SOL");
    ASSERT_EQ(result.status, OperationStatus::Verified);
    EXPECT_TRUE(result.token.verified);
    EXPECT_EQ(result.token.symbol, "SOL");
    EXPECT_EQ(result.token.dataSource, "fake-adapter");
    EXPECT_EQ(result.token.evidenceReference, "token-1");
    EXPECT_EQ(result.token.asOfEpochSeconds, kObservedAt);
    const auto cached = agent->queryTokenInfoByAddress(kSolAddress);
    EXPECT_EQ(cached.status, OperationStatus::Verified);
    EXPECT_EQ(cached.token.symbol, "SOL");
    const auto top = agent->queryTopTokenInfo(2);
    ASSERT_EQ(top.status, OperationStatus::Verified);
    ASSERT_EQ(top.tokens.size(), 2U);
    EXPECT_TRUE(top.tokens[1].verified);
}

TEST_F(SpartanAdapterTest, RejectsMalformedOrUnverifiedTokenPayloads) {
    ASSERT_TRUE(agent->initialize());
    EXPECT_EQ(agent->queryTokenInfo("bad symbol!").status, OperationStatus::Rejected);
    EXPECT_EQ(agent->queryTokenInfoByAddress("short").status, OperationStatus::Rejected);
    EXPECT_EQ(agent->queryTopTokenInfo(0).status, OperationStatus::Rejected);
    adapter->tokenResult.token.price = std::numeric_limits<double>::quiet_NaN();
    EXPECT_EQ(agent->queryTokenInfo("SOL").status, OperationStatus::Failed);
    adapter->tokenResult.token = solToken();
    adapter->tokenResult.evidence.verified = false;
    EXPECT_EQ(agent->queryTokenInfo("SOL").status, OperationStatus::Failed);
    adapter->throwOnToken = true;
    EXPECT_EQ(agent->queryTokenInfo("USDC").status, OperationStatus::Failed);
}

TEST_F(SpartanAdapterTest, VerifiedTradeReceiptAndSubmittedStateAreDistinct) {
    ASSERT_TRUE(agent->initialize());
    const auto verified = agent->executeSwap("SOL", "USDC", 5.0, "Orca", true);
    EXPECT_EQ(verified.status, OperationStatus::Verified);
    EXPECT_TRUE(verified.success);
    EXPECT_EQ(verified.transactionId, "verified-swap-signature");
    EXPECT_NE(verified.message.find("verified:"), std::string::npos);
    {
        std::lock_guard<std::mutex> lock(adapter->mutex);
        EXPECT_TRUE(adapter->seenSwap.userConfirmed);
        EXPECT_EQ(adapter->seenSwap.dex, "Orca");
    }
    adapter->swapResult.status = OperationStatus::Submitted;
    adapter->swapResult.message = "accepted by RPC";
    adapter->swapResult.evidence = {"fake-adapter", "submission-1", kObservedAt, false};
    const auto submitted = agent->executeSwap("SOL", "USDC", 5.0, "Orca", true);
    EXPECT_EQ(submitted.status, OperationStatus::Submitted);
    EXPECT_FALSE(submitted.success);
    EXPECT_EQ(submitted.amountTraded, 0.0);
    EXPECT_NE(submitted.message.find("submitted:"), std::string::npos);
}

TEST_F(SpartanAdapterTest, AdapterFailureAndFalseVerifiedReceiptAreTruthful) {
    ASSERT_TRUE(agent->initialize());
    adapter->throwOnSwap = true;
    const auto thrown = agent->executeSwap("SOL", "USDC", 5.0, "Orca", true);
    EXPECT_EQ(thrown.status, OperationStatus::Failed);
    EXPECT_FALSE(thrown.success);
    EXPECT_TRUE(thrown.transactionId.empty());
    adapter->throwOnSwap = false;
    adapter->swapResult.evidence = {};
    const auto invalid = agent->executeSwap("SOL", "USDC", 5.0, "Orca", true);
    EXPECT_EQ(invalid.status, OperationStatus::Failed);
    EXPECT_FALSE(invalid.success);
    EXPECT_TRUE(invalid.transactionId.empty());
}

TEST_F(SpartanAdapterTest, ConfirmationLimitsRiskAndDexRejectBeforeAdapterCall) {
    ASSERT_TRUE(agent->initialize());
    EXPECT_EQ(agent->executeSwap("SOL", "USDC", 5.0, "Orca").status, OperationStatus::Rejected);
    EXPECT_EQ(agent->executeSwap("SOL", "USDC", 101.0, "Orca", true).status, OperationStatus::Rejected);
    EXPECT_EQ(agent->executeSwap("SOL", "USDC", 100.0001, "Orca", true).status, OperationStatus::Rejected);
    EXPECT_EQ(agent->executeSwap("SOL", "USDC", 99.0, "Unknown", true).status, OperationStatus::Rejected);
    EXPECT_EQ(agent->executeSwap("SOL", "SOL", 1.0, "Orca", true).status, OperationStatus::Rejected);
    EXPECT_EQ(adapter->swapCalls.load(), 0);
    adapter->swapResult.amountTraded = 100.0;
    const auto boundary = agent->executeSwap("SOL", "USDC", 100.0, "Orca", true);
    EXPECT_EQ(boundary.status, OperationStatus::Verified);
    EXPECT_EQ(adapter->swapCalls.load(), 1);
}

TEST_F(SpartanAdapterTest, LiquidityReceiptsAreVerifiedAndMalformedValuesRejected) {
    ASSERT_TRUE(agent->initialize());
    EXPECT_EQ(agent->addLiquidity("SOL", "USDC", 5.0, 5.0, "Orca").status, OperationStatus::Rejected);
    EXPECT_EQ(agent->addLiquidity("SOL", "USDC", 5.0, std::numeric_limits<double>::infinity(), "Orca", true).status, OperationStatus::Rejected);
    const auto added = agent->addLiquidity("SOL", "USDC", 5.0, 5.0, "Orca", true);
    EXPECT_EQ(added.status, OperationStatus::Verified);
    EXPECT_TRUE(added.success);
    EXPECT_EQ(adapter->liquidityCalls.load(), 1);
    const auto removed = agent->removeLiquidity("SOL", "USDC", 4.0, "Raydium", true);
    EXPECT_EQ(removed.status, OperationStatus::Verified);
    EXPECT_TRUE(removed.success);
    EXPECT_EQ(adapter->liquidityCalls.load(), 2);
}

TEST_F(SpartanAdapterTest, CopyTradingMutatesStateOnlyAfterVerifiedReceipt) {
    ASSERT_TRUE(agent->initialize());
    EXPECT_EQ(agent->setupCopyTradingResult(kWallet, 0.05).status, OperationStatus::Rejected);
    adapter->copyResult.status = OperationStatus::Submitted;
    adapter->copyResult.evidence = {"fake-adapter", "copy-submission", kObservedAt, false};
    EXPECT_EQ(agent->setupCopyTradingResult(kWallet, 0.05, true).status, OperationStatus::Submitted);
    EXPECT_TRUE(agent->getCopyTradingWallets().empty());
    adapter->copyResult.status = OperationStatus::Verified;
    adapter->copyResult.evidence = verifiedEvidence("copy-start");
    EXPECT_TRUE(agent->setupCopyTrading(kWallet, 0.05, true));
    ASSERT_EQ(agent->getCopyTradingWallets().size(), 1U);
    EXPECT_FALSE(agent->setupCopyTrading(kWallet, 0.05, true));
    adapter->copyResult.evidence = verifiedEvidence("copy-stop");
    EXPECT_TRUE(agent->stopCopyTrading(kWallet, true));
    EXPECT_TRUE(agent->getCopyTradingWallets().empty());
    EXPECT_FALSE(agent->stopCopyTrading(kWallet, true));
}

TEST_F(SpartanAdapterTest, ShutdownAndRestartDisconnectAndReconnectCleanly) {
    ASSERT_TRUE(agent->initialize());
    EXPECT_TRUE(agent->isExternallyConnected());
    EXPECT_EQ(adapter->connectCalls.load(), 1);
    agent->shutdown();
    EXPECT_FALSE(agent->isInitialized());
    EXPECT_FALSE(agent->isExternallyConnected());
    EXPECT_EQ(adapter->disconnectCalls.load(), 1);
    EXPECT_EQ(agent->queryTokenInfo("SOL").status, OperationStatus::Rejected);
    EXPECT_TRUE(agent->initialize());
    EXPECT_TRUE(agent->isExternallyConnected());
    EXPECT_EQ(adapter->connectCalls.load(), 2);
}

TEST_F(SpartanAdapterTest, VerifiedFormattingIsTimestampedAndNeverCalledCurrent) {
    ASSERT_TRUE(agent->initialize());
    const auto token = agent->queryTokenInfo("SOL").token;
    const std::string formatted = agent->formatTokenPrice(token);
    EXPECT_NE(formatted.find("Verified SOL snapshot"), std::string::npos);
    EXPECT_NE(formatted.find("observed_at_epoch: 1700000000"), std::string::npos);
    EXPECT_EQ(formatted.find("Current"), std::string::npos);
    EXPECT_EQ(formatted.find("APR"), std::string::npos);
}

} // namespace
} // namespace elizaos
