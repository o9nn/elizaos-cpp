#pragma once

#include "elizaos/core.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

/** Configuration for Spartan's local policy and an optional injected adapter. */
struct SpartanConfig {
    std::string solanaRpcUrl;
    std::string solanaPublicKey;
    // Deprecated compatibility field. SpartanAgent deliberately does not retain,
    // return, log, or forward this value; credentials belong inside an adapter.
    std::string solanaPrivateKey;
    std::vector<std::string> preferredDexes;
    double maxTradeAmount = 1000.0;
    double maxRiskPercentage = 0.1;
    // When positive, amounts and this value are treated as the caller's common
    // risk unit. External actions are rejected when the ratio exceeds policy.
    double portfolioValue = 0.0;
    bool requireConfirmation = true;

    SpartanConfig() = default;
    SpartanConfig(const std::string& rpcUrl,
                  const std::string& publicKey,
                  const std::string& privateKey)
        : solanaRpcUrl(rpcUrl),
          solanaPublicKey(publicKey),
          solanaPrivateKey(privateKey),
          preferredDexes{"Orca", "Raydium", "Meteora"} {}
};

enum class OperationStatus {
    Unsupported,
    Rejected,
    Submitted,
    Verified,
    Failed
};

const char* operationStatusName(OperationStatus status) noexcept;

/** Evidence supplied by an adapter. verified=true means independently checked. */
struct AdapterEvidence {
    std::string provider;
    std::string reference;
    std::int64_t observedAtEpochSeconds = 0;
    bool verified = false;
};

struct ConnectionResult {
    OperationStatus status = OperationStatus::Unsupported;
    std::string message;
    AdapterEvidence evidence;

    bool connected() const noexcept { return status == OperationStatus::Verified; }
};

/** A point-in-time token snapshot. Zero fields are not interpreted as live data. */
struct TokenInfo {
    std::string symbol;
    std::string address;
    double price = 0.0;
    double marketCap = 0.0;
    double volume24h = 0.0;
    double change24h = 0.0;
    std::int64_t asOfEpochSeconds = 0;
    std::string dataSource;
    std::string evidenceReference;
    bool verified = false;

    TokenInfo() = default;
    TokenInfo(const std::string& sym, const std::string& addr)
        : symbol(sym), address(addr) {}
};

struct TokenQueryResult {
    OperationStatus status = OperationStatus::Unsupported;
    std::string message;
    TokenInfo token;
    std::vector<TokenInfo> tokens;
    AdapterEvidence evidence;

    bool verified() const noexcept { return status == OperationStatus::Verified; }
};

/** Deterministic, local-only pool bookkeeping. It does not represent an LP. */
struct TradingPool {
    std::string id;
    std::string name;
    std::vector<std::string> owners;
    double totalValue = 0.0;
    std::unordered_map<std::string, double> allocations;

    TradingPool() = default;
    TradingPool(const std::string& poolId, const std::string& poolName)
        : id(poolId), name(poolName) {}
};

struct TradeResult {
    // Legacy success is true only for a strictly verified receipt, never merely
    // because an adapter accepted or submitted an action.
    bool success = false;
    OperationStatus status = OperationStatus::Unsupported;
    std::string transactionId;
    std::string message;
    double amountTraded = 0.0;
    double executionPrice = 0.0;
    AdapterEvidence evidence;

    TradeResult() = default;
    TradeResult(bool succeeded, const std::string& txId, const std::string& msg)
        : success(succeeded),
          status(succeeded ? OperationStatus::Verified : OperationStatus::Rejected),
          transactionId(txId),
          message(msg) {}
};

struct OperationResult {
    OperationStatus status = OperationStatus::Unsupported;
    std::string message;
    AdapterEvidence evidence;

    bool verified() const noexcept { return status == OperationStatus::Verified; }
};

struct SwapRequest {
    std::string fromToken;
    std::string toToken;
    double amount = 0.0;
    std::string dex;
    bool userConfirmed = false;
};

enum class LiquidityAction { Add, Remove };

struct LiquidityRequest {
    LiquidityAction action = LiquidityAction::Add;
    std::string tokenA;
    std::string tokenB;
    double amountA = 0.0;
    double amountB = 0.0;
    std::string dex;
    bool userConfirmed = false;
};

enum class CopyTradingAction { Start, Stop };

struct CopyTradingRequest {
    CopyTradingAction action = CopyTradingAction::Start;
    std::string walletAddress;
    double allocation = 0.0;
    bool userConfirmed = false;
};

/**
 * Explicit external boundary. Spartan ships with no implementation of this
 * interface and therefore performs no network access or financial action.
 * Implementations must return evidence-backed statuses; Spartan re-validates
 * all responses before exposing them as verified.
 */
class ISpartanAdapter {
public:
    virtual ~ISpartanAdapter() = default;
    virtual ConnectionResult connect(const SpartanConfig& sanitizedConfig) = 0;
    virtual void disconnect() noexcept = 0;
    virtual TokenQueryResult queryTokenBySymbol(const std::string& symbol) = 0;
    virtual TokenQueryResult queryTokenByAddress(const std::string& address) = 0;
    virtual TokenQueryResult queryTopTokens(std::size_t count) = 0;
    virtual TradeResult executeSwap(const SwapRequest& request) = 0;
    virtual TradeResult changeLiquidity(const LiquidityRequest& request) = 0;
    virtual OperationResult configureCopyTrading(const CopyTradingRequest& request) = 0;
};

/** Solana trading facade with local policy enforcement and injected I/O only. */
class SpartanAgent {
public:
    explicit SpartanAgent(const SpartanConfig& config,
                          std::shared_ptr<ISpartanAdapter> adapter = nullptr);
    virtual ~SpartanAgent();

    // initialize() reports local readiness. Inspect isExternallyConnected() or
    // getConnectionStatus() separately; no adapter is a truthful local-only mode.
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    bool isExternallyConnected() const;
    ConnectionResult getConnectionStatus() const;

    // Returned config is a copy with solanaPrivateKey always empty.
    SpartanConfig getConfig() const;
    bool updateConfig(const SpartanConfig& config);
    // Adapter replacement is accepted only while shut down.
    bool setAdapter(std::shared_ptr<ISpartanAdapter> adapter);

    // Local deterministic pool bookkeeping.
    std::string createTradingPool(const std::string& name,
                                  const std::vector<std::string>& owners);
    bool deleteTradingPool(const std::string& poolId);
    bool addPoolOwner(const std::string& poolId, const std::string& owner);
    bool removePoolOwner(const std::string& poolId, const std::string& owner);
    bool setPoolAllocation(const std::string& poolId,
                           const std::string& token,
                           double allocation);
    std::vector<TradingPool> getTradingPools() const;
    std::optional<TradingPool> getTradingPoolSnapshot(const std::string& poolId) const;
    // Compatibility view: points to a per-thread snapshot, never mutable internal
    // storage. The pointer is replaced by the next call on the same thread.
    TradingPool* getTradingPool(const std::string& poolId);

    // Evidence-bearing APIs are authoritative. Legacy value APIs return empty
    // values rather than inventing market data when verification is unavailable.
    TokenQueryResult queryTokenInfo(const std::string& symbol);
    TokenQueryResult queryTokenInfoByAddress(const std::string& address);
    TokenQueryResult queryTopTokenInfo(std::size_t count = 10);
    TokenInfo getTokenInfo(const std::string& symbol);
    TokenInfo getTokenInfoByAddress(const std::string& address);
    std::vector<TokenInfo> getTopTokens(std::size_t count = 10);

    TradeResult executeSwap(const std::string& fromToken,
                            const std::string& toToken,
                            double amount,
                            const std::string& dex = "",
                            bool confirmed = false);
    TradeResult addLiquidity(const std::string& tokenA,
                             const std::string& tokenB,
                             double amountA,
                             double amountB,
                             const std::string& dex = "",
                             bool confirmed = false);
    TradeResult removeLiquidity(const std::string& tokenA,
                                const std::string& tokenB,
                                double lpTokens,
                                const std::string& dex = "",
                                bool confirmed = false);

    OperationResult setupCopyTradingResult(const std::string& walletAddress,
                                           double allocation,
                                           bool confirmed = false);
    OperationResult stopCopyTradingResult(const std::string& walletAddress,
                                          bool confirmed = false);
    bool setupCopyTrading(const std::string& walletAddress,
                          double allocation,
                          bool confirmed = false);
    bool stopCopyTrading(const std::string& walletAddress,
                         bool confirmed = false);
    std::vector<std::string> getCopyTradingWallets() const;

    bool validateTrade(const std::string& fromToken,
                       const std::string& toToken,
                       double amount) const;
    double calculateRiskPercentage(double amount, double portfolioValue) const;

    std::string generateResponse(const std::string& query);
    std::string formatTokenPrice(const TokenInfo& token) const;
    bool requiresConfirmation(const std::string& action) const;

private:
    struct ExternalSnapshot {
        std::shared_ptr<ISpartanAdapter> adapter;
        std::uint64_t generation = 0;
        bool ready = false;
        OperationStatus failureStatus = OperationStatus::Unsupported;
        std::string failureMessage;
    };

    mutable std::mutex stateMutex_;
    mutable std::mutex adapterCallMutex_;
    SpartanConfig config_;
    bool initialized_ = false;
    bool externallyConnected_ = false;
    ConnectionResult connectionStatus_;
    std::shared_ptr<ISpartanAdapter> adapter_;
    std::uint64_t generation_ = 0;
    std::uint64_t nextPoolId_ = 1;
    std::map<std::string, TradingPool> pools_;
    std::unordered_map<std::string, TokenInfo> tokenCache_;
    std::map<std::string, double> copyTradingWallets_;

    static SpartanConfig sanitizeConfig(const SpartanConfig& config);
    static bool validateConfigValue(const SpartanConfig& config);
    static bool isValidSolanaAddress(const std::string& value);
    static bool isValidIdentifier(const std::string& value);
    static bool isValidToken(const std::string& value);
    static bool isValidDexName(const std::string& value);
    static bool isValidPoolId(const std::string& value);
    static bool isFinitePositive(double value);
    static bool isEvidenceValid(const AdapterEvidence& evidence, bool requireVerified);
    static std::string statusMessage(OperationStatus status, const std::string& detail);

    std::string selectDexLocked(const std::string& dex) const;
    bool validateTradeLocked(const std::string& fromToken,
                             const std::string& toToken,
                             double amount) const;
    ExternalSnapshot externalSnapshotLocked() const;
    bool generationStillCurrent(const ExternalSnapshot& snapshot) const;
    std::string generatePoolIdLocked();
    void updateTokenCacheLocked(const TokenInfo& token);
    TokenQueryResult normalizeTokenResult(TokenQueryResult result,
                                          const std::string& expected,
                                          bool byAddress,
                                          std::size_t maxCount,
                                          const ExternalSnapshot& snapshot);
    TradeResult normalizeTradeResult(TradeResult result,
                                     double requestedAmount,
                                     const ExternalSnapshot& snapshot);
    OperationResult normalizeOperationResult(OperationResult result,
                                             const ExternalSnapshot& snapshot);
};

std::unique_ptr<SpartanAgent> createSpartanAgent(
    const SpartanConfig& config,
    std::shared_ptr<ISpartanAdapter> adapter = nullptr);
SpartanConfig getDefaultSpartanConfig();
SpartanConfig getSpartanConfigFromEnvironment();

} // namespace elizaos
