#include "elizaos/spartan.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>
#include <utility>

namespace elizaos {
namespace {

constexpr std::size_t kMaxNameLength = 128;
constexpr std::size_t kMaxIdentifierLength = 128;
constexpr std::size_t kMaxTopTokens = 100;
constexpr std::size_t kSolanaAddressMinLength = 32;
constexpr std::size_t kSolanaAddressMaxLength = 44;

bool isAsciiSpace(unsigned char c) {
    return std::isspace(c) != 0;
}

std::string trim(const std::string& value) {
    const auto first = std::find_if_not(value.begin(), value.end(),
                                        [](unsigned char c) { return isAsciiSpace(c); });
    const auto last = std::find_if_not(value.rbegin(), value.rend(),
                                       [](unsigned char c) { return isAsciiSpace(c); }).base();
    return first < last ? std::string(first, last) : std::string();
}

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool isBase58Character(char c) {
    static const std::string alphabet =
        "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    return alphabet.find(c) != std::string::npos;
}

bool isAllowedIdentifierCharacter(unsigned char c) {
    return std::isalnum(c) != 0 || c == '_' || c == '-' || c == '.' || c == ':';
}

bool isAllowedTokenCharacter(unsigned char c) {
    return std::isalnum(c) != 0 || c == '_' || c == '-' || c == '.';
}

bool isHttpUrl(const std::string& value) {
    const std::string lower = lowercase(value);
    const std::size_t schemeLength = lower.rfind("https://", 0) == 0 ? 8U
        : (lower.rfind("http://", 0) == 0 ? 7U : 0U);
    if (schemeLength == 0 || value.size() <= schemeLength ||
        value.find_first_of(" \t\r\n#") != std::string::npos) {
        return false;
    }
    const std::size_t authorityEnd = value.find_first_of("/?", schemeLength);
    const std::string authority = value.substr(schemeLength,
        authorityEnd == std::string::npos ? std::string::npos : authorityEnd - schemeLength);
    return !authority.empty() && authority.find('@') == std::string::npos;
}

TradeResult makeTradeResult(OperationStatus status, const std::string& detail) {
    TradeResult result;
    result.status = status;
    result.success = false;
    result.message = std::string(operationStatusName(status)) + ": " + detail;
    return result;
}

OperationResult makeOperationResult(OperationStatus status, const std::string& detail) {
    OperationResult result;
    result.status = status;
    result.message = std::string(operationStatusName(status)) + ": " + detail;
    return result;
}

TokenQueryResult makeTokenResult(OperationStatus status, const std::string& detail) {
    TokenQueryResult result;
    result.status = status;
    result.message = std::string(operationStatusName(status)) + ": " + detail;
    return result;
}

bool hasForbiddenMarketClaim(const std::string& value) {
    const std::string lower = lowercase(value);
    return lower.find("current ") != std::string::npos ||
           lower.find("live ") != std::string::npos ||
           lower.find("latest ") != std::string::npos ||
           lower.find(" apr") != std::string::npos;
}

} // namespace

const char* operationStatusName(OperationStatus status) noexcept {
    switch (status) {
    case OperationStatus::Unsupported:
        return "unsupported";
    case OperationStatus::Rejected:
        return "rejected";
    case OperationStatus::Submitted:
        return "submitted";
    case OperationStatus::Verified:
        return "verified";
    case OperationStatus::Failed:
        return "failed";
    }
    return "failed";
}

SpartanAgent::SpartanAgent(const SpartanConfig& config,
                           std::shared_ptr<ISpartanAdapter> adapter)
    : config_(sanitizeConfig(config)), adapter_(std::move(adapter)) {
    connectionStatus_.status = OperationStatus::Unsupported;
    connectionStatus_.message = statusMessage(
        OperationStatus::Unsupported,
        "external connectivity has not been attempted; local mode only");
}

SpartanAgent::~SpartanAgent() {
    shutdown();
}

bool SpartanAgent::initialize() {
    std::shared_ptr<ISpartanAdapter> adapter;
    SpartanConfig config;
    std::uint64_t generation = 0;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (initialized_) {
            return true;
        }
        if (!validateConfigValue(config_)) {
            connectionStatus_.status = OperationStatus::Rejected;
            connectionStatus_.message = statusMessage(
                OperationStatus::Rejected, "invalid Spartan configuration");
            return false;
        }

        initialized_ = true;
        externallyConnected_ = false;
        ++generation_;
        generation = generation_;
        adapter = adapter_;
        config = config_;
        config.solanaPrivateKey.clear();
        if (!adapter) {
            connectionStatus_.status = OperationStatus::Unsupported;
            connectionStatus_.message = statusMessage(
                OperationStatus::Unsupported,
                "local bookkeeping is ready; no external adapter is installed");
            connectionStatus_.evidence = {};
            return true;
        }
    }

    ConnectionResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = adapter->connect(config);
    } catch (const std::exception& ex) {
        result.status = OperationStatus::Failed;
        result.message = statusMessage(OperationStatus::Failed,
                                       std::string("adapter connection exception: ") + ex.what());
    } catch (...) {
        result.status = OperationStatus::Failed;
        result.message = statusMessage(OperationStatus::Failed,
                                       "adapter connection raised an unknown exception");
    }

    if (result.connected() && !isEvidenceValid(result.evidence, true)) {
        result.status = OperationStatus::Failed;
        result.message = statusMessage(
            OperationStatus::Failed,
            "adapter claimed connectivity without verifiable evidence");
    } else if (result.message.empty()) {
        result.message = statusMessage(result.status, result.connected()
            ? "external connectivity verified"
            : "external connectivity was not verified");
    } else if (lowercase(result.message).rfind(operationStatusName(result.status), 0) != 0) {
        result.message = statusMessage(result.status, result.message);
    }

    bool becameStale = false;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        becameStale = !initialized_ || generation_ != generation || adapter_ != adapter;
        if (!becameStale) {
            externallyConnected_ = result.connected();
            connectionStatus_ = result;
        }
    }
    if (becameStale && result.connected()) {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        adapter->disconnect();
    }
    return true;
}

void SpartanAgent::shutdown() {
    std::shared_ptr<ISpartanAdapter> adapter;
    bool shouldDisconnect = false;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!initialized_) {
            return;
        }
        adapter = adapter_;
        shouldDisconnect = externallyConnected_ && static_cast<bool>(adapter);
        initialized_ = false;
        externallyConnected_ = false;
        ++generation_;
        tokenCache_.clear();
        copyTradingWallets_.clear();
        connectionStatus_.status = OperationStatus::Unsupported;
        connectionStatus_.message = statusMessage(
            OperationStatus::Unsupported, "agent is shut down; external actions are unavailable");
        connectionStatus_.evidence = {};
    }
    if (shouldDisconnect) {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        adapter->disconnect();
    }
}

bool SpartanAgent::isInitialized() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return initialized_;
}

bool SpartanAgent::isExternallyConnected() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return externallyConnected_;
}

ConnectionResult SpartanAgent::getConnectionStatus() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return connectionStatus_;
}

SpartanConfig SpartanAgent::getConfig() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    SpartanConfig copy = config_;
    copy.solanaPrivateKey.clear();
    return copy;
}

bool SpartanAgent::updateConfig(const SpartanConfig& config) {
    SpartanConfig sanitized = sanitizeConfig(config);
    if (!validateConfigValue(sanitized)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (initialized_) {
        return false;
    }
    config_ = std::move(sanitized);
    return true;
}

bool SpartanAgent::setAdapter(std::shared_ptr<ISpartanAdapter> adapter) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (initialized_) {
        return false;
    }
    adapter_ = std::move(adapter);
    externallyConnected_ = false;
    connectionStatus_ = {};
    connectionStatus_.message = statusMessage(
        OperationStatus::Unsupported, "external connectivity has not been attempted");
    return true;
}

std::string SpartanAgent::createTradingPool(const std::string& name,
                                             const std::vector<std::string>& owners) {
    const std::string cleanName = trim(name);
    if (cleanName.empty() || cleanName.size() > kMaxNameLength || owners.empty()) {
        return {};
    }

    std::vector<std::string> normalizedOwners;
    normalizedOwners.reserve(owners.size());
    std::set<std::string> seen;
    for (const auto& owner : owners) {
        const std::string cleanOwner = trim(owner);
        if (!isValidIdentifier(cleanOwner) || !seen.insert(cleanOwner).second) {
            return {};
        }
        normalizedOwners.push_back(cleanOwner);
    }
    std::sort(normalizedOwners.begin(), normalizedOwners.end());

    std::lock_guard<std::mutex> lock(stateMutex_);
    if (!initialized_) {
        return {};
    }
    const std::string id = generatePoolIdLocked();
    TradingPool pool(id, cleanName);
    pool.owners = std::move(normalizedOwners);
    pools_.emplace(id, std::move(pool));
    return id;
}

bool SpartanAgent::deleteTradingPool(const std::string& poolId) {
    if (!isValidPoolId(poolId)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(stateMutex_);
    return initialized_ && pools_.erase(poolId) == 1;
}

bool SpartanAgent::addPoolOwner(const std::string& poolId, const std::string& owner) {
    const std::string cleanOwner = trim(owner);
    if (!isValidPoolId(poolId) || !isValidIdentifier(cleanOwner)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (!initialized_) {
        return false;
    }
    auto it = pools_.find(poolId);
    if (it == pools_.end()) {
        return false;
    }
    auto& owners = it->second.owners;
    if (std::find(owners.begin(), owners.end(), cleanOwner) != owners.end()) {
        return false;
    }
    owners.push_back(cleanOwner);
    std::sort(owners.begin(), owners.end());
    return true;
}

bool SpartanAgent::removePoolOwner(const std::string& poolId, const std::string& owner) {
    const std::string cleanOwner = trim(owner);
    if (!isValidPoolId(poolId) || !isValidIdentifier(cleanOwner)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (!initialized_) {
        return false;
    }
    auto it = pools_.find(poolId);
    if (it == pools_.end() || it->second.owners.size() <= 1) {
        return false;
    }
    auto& owners = it->second.owners;
    const auto ownerIt = std::find(owners.begin(), owners.end(), cleanOwner);
    if (ownerIt == owners.end()) {
        return false;
    }
    owners.erase(ownerIt);
    return true;
}

bool SpartanAgent::setPoolAllocation(const std::string& poolId,
                                      const std::string& token,
                                      double allocation) {
    const std::string cleanToken = trim(token);
    if (!isValidPoolId(poolId) || !isValidToken(cleanToken) ||
        !std::isfinite(allocation) || allocation < 0.0 || allocation > 1.0) {
        return false;
    }

    std::lock_guard<std::mutex> lock(stateMutex_);
    if (!initialized_) {
        return false;
    }
    auto poolIt = pools_.find(poolId);
    if (poolIt == pools_.end()) {
        return false;
    }

    auto proposed = poolIt->second.allocations;
    if (allocation == 0.0) {
        proposed.erase(cleanToken);
    } else {
        proposed[cleanToken] = allocation;
    }
    double sum = 0.0;
    for (const auto& entry : proposed) {
        sum += entry.second;
    }
    if (!std::isfinite(sum) || sum > 1.0 + 1e-12) {
        return false;
    }
    poolIt->second.allocations = std::move(proposed);
    return true;
}

std::vector<TradingPool> SpartanAgent::getTradingPools() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    std::vector<TradingPool> result;
    result.reserve(pools_.size());
    for (const auto& entry : pools_) {
        result.push_back(entry.second);
    }
    return result;
}

std::optional<TradingPool> SpartanAgent::getTradingPoolSnapshot(
    const std::string& poolId) const {
    if (!isValidPoolId(poolId)) {
        return std::nullopt;
    }
    std::lock_guard<std::mutex> lock(stateMutex_);
    const auto it = pools_.find(poolId);
    if (it == pools_.end()) {
        return std::nullopt;
    }
    return it->second;
}

TradingPool* SpartanAgent::getTradingPool(const std::string& poolId) {
    thread_local TradingPool snapshot;
    const auto value = getTradingPoolSnapshot(poolId);
    if (!value) {
        return nullptr;
    }
    snapshot = *value;
    return &snapshot;
}

TokenQueryResult SpartanAgent::queryTokenInfo(const std::string& symbol) {
    const std::string cleanSymbol = trim(symbol);
    if (!isValidToken(cleanSymbol)) {
        return makeTokenResult(OperationStatus::Rejected, "malformed token symbol");
    }

    ExternalSnapshot snapshot;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        snapshot = externalSnapshotLocked();
    }
    if (!snapshot.ready) {
        return makeTokenResult(snapshot.failureStatus, snapshot.failureMessage);
    }

    TokenQueryResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = snapshot.adapter->queryTokenBySymbol(cleanSymbol);
    } catch (const std::exception& ex) {
        return makeTokenResult(OperationStatus::Failed,
                               std::string("token adapter exception: ") + ex.what());
    } catch (...) {
        return makeTokenResult(OperationStatus::Failed,
                               "token adapter raised an unknown exception");
    }
    return normalizeTokenResult(std::move(result), cleanSymbol, false, 1, snapshot);
}

TokenQueryResult SpartanAgent::queryTokenInfoByAddress(const std::string& address) {
    const std::string cleanAddress = trim(address);
    if (!isValidSolanaAddress(cleanAddress)) {
        return makeTokenResult(OperationStatus::Rejected, "malformed Solana token address");
    }

    ExternalSnapshot snapshot;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        const auto cached = tokenCache_.find(cleanAddress);
        if (cached != tokenCache_.end()) {
            TokenQueryResult cachedResult;
            cachedResult.status = OperationStatus::Verified;
            cachedResult.message = statusMessage(OperationStatus::Verified,
                                                  "verified token snapshot returned from cache");
            cachedResult.token = cached->second;
            cachedResult.evidence = {cached->second.dataSource,
                                     cached->second.evidenceReference,
                                     cached->second.asOfEpochSeconds,
                                     true};
            return cachedResult;
        }
        snapshot = externalSnapshotLocked();
    }
    if (!snapshot.ready) {
        return makeTokenResult(snapshot.failureStatus, snapshot.failureMessage);
    }

    TokenQueryResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = snapshot.adapter->queryTokenByAddress(cleanAddress);
    } catch (const std::exception& ex) {
        return makeTokenResult(OperationStatus::Failed,
                               std::string("token adapter exception: ") + ex.what());
    } catch (...) {
        return makeTokenResult(OperationStatus::Failed,
                               "token adapter raised an unknown exception");
    }
    return normalizeTokenResult(std::move(result), cleanAddress, true, 1, snapshot);
}

TokenQueryResult SpartanAgent::queryTopTokenInfo(std::size_t count) {
    if (count == 0 || count > kMaxTopTokens) {
        return makeTokenResult(OperationStatus::Rejected,
                               "top-token count must be between 1 and 100");
    }
    ExternalSnapshot snapshot;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        snapshot = externalSnapshotLocked();
    }
    if (!snapshot.ready) {
        return makeTokenResult(snapshot.failureStatus, snapshot.failureMessage);
    }

    TokenQueryResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = snapshot.adapter->queryTopTokens(count);
    } catch (const std::exception& ex) {
        return makeTokenResult(OperationStatus::Failed,
                               std::string("token adapter exception: ") + ex.what());
    } catch (...) {
        return makeTokenResult(OperationStatus::Failed,
                               "token adapter raised an unknown exception");
    }
    return normalizeTokenResult(std::move(result), {}, false, count, snapshot);
}

TokenInfo SpartanAgent::getTokenInfo(const std::string& symbol) {
    const auto result = queryTokenInfo(symbol);
    return result.verified() ? result.token : TokenInfo{};
}

TokenInfo SpartanAgent::getTokenInfoByAddress(const std::string& address) {
    const auto result = queryTokenInfoByAddress(address);
    return result.verified() ? result.token : TokenInfo{};
}

std::vector<TokenInfo> SpartanAgent::getTopTokens(std::size_t count) {
    const auto result = queryTopTokenInfo(count);
    return result.verified() ? result.tokens : std::vector<TokenInfo>{};
}

TradeResult SpartanAgent::executeSwap(const std::string& fromToken,
                                       const std::string& toToken,
                                       double amount,
                                       const std::string& dex,
                                       bool confirmed) {
    ExternalSnapshot snapshot;
    SwapRequest request;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!validateTradeLocked(fromToken, toToken, amount)) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "trade violates token, amount, or risk policy");
        }
        request.fromToken = trim(fromToken);
        request.toToken = trim(toToken);
        request.amount = amount;
        request.dex = selectDexLocked(dex);
        request.userConfirmed = confirmed;
        if (request.dex.empty()) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "DEX is malformed or not in preferredDexes");
        }
        if (config_.requireConfirmation && !confirmed) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "explicit user confirmation is required for swap");
        }
        snapshot = externalSnapshotLocked();
    }
    if (!snapshot.ready) {
        return makeTradeResult(snapshot.failureStatus, snapshot.failureMessage);
    }

    TradeResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = snapshot.adapter->executeSwap(request);
    } catch (const std::exception& ex) {
        return makeTradeResult(OperationStatus::Failed,
                               std::string("swap adapter exception: ") + ex.what());
    } catch (...) {
        return makeTradeResult(OperationStatus::Failed,
                               "swap adapter raised an unknown exception");
    }
    return normalizeTradeResult(std::move(result), amount, snapshot);
}

TradeResult SpartanAgent::addLiquidity(const std::string& tokenA,
                                        const std::string& tokenB,
                                        double amountA,
                                        double amountB,
                                        const std::string& dex,
                                        bool confirmed) {
    ExternalSnapshot snapshot;
    LiquidityRequest request;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!isValidToken(trim(tokenA)) || !isValidToken(trim(tokenB)) ||
            trim(tokenA) == trim(tokenB) || !isFinitePositive(amountA) ||
            !isFinitePositive(amountB) || amountA > config_.maxTradeAmount ||
            amountB > config_.maxTradeAmount ||
            (config_.portfolioValue > 0.0 &&
             calculateRiskPercentage(amountA + amountB, config_.portfolioValue) >
                 config_.maxRiskPercentage)) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "liquidity request violates token, amount, or risk policy");
        }
        request = {LiquidityAction::Add, trim(tokenA), trim(tokenB), amountA,
                   amountB, selectDexLocked(dex), confirmed};
        if (request.dex.empty()) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "DEX is malformed or not in preferredDexes");
        }
        if (config_.requireConfirmation && !confirmed) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "explicit user confirmation is required for liquidity change");
        }
        snapshot = externalSnapshotLocked();
    }
    if (!snapshot.ready) {
        return makeTradeResult(snapshot.failureStatus, snapshot.failureMessage);
    }

    TradeResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = snapshot.adapter->changeLiquidity(request);
    } catch (const std::exception& ex) {
        return makeTradeResult(OperationStatus::Failed,
                               std::string("liquidity adapter exception: ") + ex.what());
    } catch (...) {
        return makeTradeResult(OperationStatus::Failed,
                               "liquidity adapter raised an unknown exception");
    }
    return normalizeTradeResult(std::move(result), amountA + amountB, snapshot);
}

TradeResult SpartanAgent::removeLiquidity(const std::string& tokenA,
                                           const std::string& tokenB,
                                           double lpTokens,
                                           const std::string& dex,
                                           bool confirmed) {
    ExternalSnapshot snapshot;
    LiquidityRequest request;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!isValidToken(trim(tokenA)) || !isValidToken(trim(tokenB)) ||
            trim(tokenA) == trim(tokenB) || !isFinitePositive(lpTokens) ||
            lpTokens > config_.maxTradeAmount) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "liquidity removal violates token or amount policy");
        }
        request = {LiquidityAction::Remove, trim(tokenA), trim(tokenB), lpTokens,
                   0.0, selectDexLocked(dex), confirmed};
        if (request.dex.empty()) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "DEX is malformed or not in preferredDexes");
        }
        if (config_.requireConfirmation && !confirmed) {
            return makeTradeResult(OperationStatus::Rejected,
                                   "explicit user confirmation is required for liquidity change");
        }
        snapshot = externalSnapshotLocked();
    }
    if (!snapshot.ready) {
        return makeTradeResult(snapshot.failureStatus, snapshot.failureMessage);
    }

    TradeResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = snapshot.adapter->changeLiquidity(request);
    } catch (const std::exception& ex) {
        return makeTradeResult(OperationStatus::Failed,
                               std::string("liquidity adapter exception: ") + ex.what());
    } catch (...) {
        return makeTradeResult(OperationStatus::Failed,
                               "liquidity adapter raised an unknown exception");
    }
    return normalizeTradeResult(std::move(result), lpTokens, snapshot);
}

OperationResult SpartanAgent::setupCopyTradingResult(const std::string& walletAddress,
                                                      double allocation,
                                                      bool confirmed) {
    const std::string cleanWallet = trim(walletAddress);
    ExternalSnapshot snapshot;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!isValidSolanaAddress(cleanWallet) || !std::isfinite(allocation) ||
            allocation <= 0.0 || allocation > 1.0 ||
            allocation > config_.maxRiskPercentage) {
            return makeOperationResult(OperationStatus::Rejected,
                                       "copy-trading wallet or allocation violates policy");
        }
        if (copyTradingWallets_.count(cleanWallet) != 0) {
            return makeOperationResult(OperationStatus::Rejected,
                                       "copy trading is already configured for wallet");
        }
        if (config_.requireConfirmation && !confirmed) {
            return makeOperationResult(OperationStatus::Rejected,
                                       "explicit user confirmation is required for copy trading");
        }
        snapshot = externalSnapshotLocked();
    }
    if (!snapshot.ready) {
        return makeOperationResult(snapshot.failureStatus, snapshot.failureMessage);
    }

    OperationResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = snapshot.adapter->configureCopyTrading(
            {CopyTradingAction::Start, cleanWallet, allocation, confirmed});
    } catch (const std::exception& ex) {
        return makeOperationResult(OperationStatus::Failed,
                                   std::string("copy-trading adapter exception: ") + ex.what());
    } catch (...) {
        return makeOperationResult(OperationStatus::Failed,
                                   "copy-trading adapter raised an unknown exception");
    }
    result = normalizeOperationResult(std::move(result), snapshot);
    if (result.verified()) {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (generationStillCurrent(snapshot)) {
            copyTradingWallets_[cleanWallet] = allocation;
        } else {
            result = makeOperationResult(OperationStatus::Failed,
                                         "agent lifecycle changed before receipt could be recorded");
        }
    }
    return result;
}

OperationResult SpartanAgent::stopCopyTradingResult(const std::string& walletAddress,
                                                     bool confirmed) {
    const std::string cleanWallet = trim(walletAddress);
    ExternalSnapshot snapshot;
    double allocation = 0.0;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!isValidSolanaAddress(cleanWallet)) {
            return makeOperationResult(OperationStatus::Rejected,
                                       "malformed copy-trading wallet address");
        }
        const auto it = copyTradingWallets_.find(cleanWallet);
        if (it == copyTradingWallets_.end()) {
            return makeOperationResult(OperationStatus::Rejected,
                                       "copy trading is not configured for wallet");
        }
        allocation = it->second;
        if (config_.requireConfirmation && !confirmed) {
            return makeOperationResult(OperationStatus::Rejected,
                                       "explicit user confirmation is required for copy trading");
        }
        snapshot = externalSnapshotLocked();
    }
    if (!snapshot.ready) {
        return makeOperationResult(snapshot.failureStatus, snapshot.failureMessage);
    }

    OperationResult result;
    try {
        std::lock_guard<std::mutex> callLock(adapterCallMutex_);
        result = snapshot.adapter->configureCopyTrading(
            {CopyTradingAction::Stop, cleanWallet, allocation, confirmed});
    } catch (const std::exception& ex) {
        return makeOperationResult(OperationStatus::Failed,
                                   std::string("copy-trading adapter exception: ") + ex.what());
    } catch (...) {
        return makeOperationResult(OperationStatus::Failed,
                                   "copy-trading adapter raised an unknown exception");
    }
    result = normalizeOperationResult(std::move(result), snapshot);
    if (result.verified()) {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (generationStillCurrent(snapshot)) {
            copyTradingWallets_.erase(cleanWallet);
        } else {
            result = makeOperationResult(OperationStatus::Failed,
                                         "agent lifecycle changed before receipt could be recorded");
        }
    }
    return result;
}

bool SpartanAgent::setupCopyTrading(const std::string& walletAddress,
                                     double allocation,
                                     bool confirmed) {
    return setupCopyTradingResult(walletAddress, allocation, confirmed).verified();
}

bool SpartanAgent::stopCopyTrading(const std::string& walletAddress, bool confirmed) {
    return stopCopyTradingResult(walletAddress, confirmed).verified();
}

std::vector<std::string> SpartanAgent::getCopyTradingWallets() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    std::vector<std::string> wallets;
    wallets.reserve(copyTradingWallets_.size());
    for (const auto& entry : copyTradingWallets_) {
        wallets.push_back(entry.first);
    }
    return wallets;
}

bool SpartanAgent::validateTrade(const std::string& fromToken,
                                  const std::string& toToken,
                                  double amount) const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return validateTradeLocked(fromToken, toToken, amount);
}

double SpartanAgent::calculateRiskPercentage(double amount, double portfolioValue) const {
    if (!std::isfinite(amount) || !std::isfinite(portfolioValue) || amount < 0.0 ||
        portfolioValue <= 0.0) {
        return std::numeric_limits<double>::infinity();
    }
    return amount / portfolioValue;
}

std::string SpartanAgent::generateResponse(const std::string& query) {
    const std::string lowerQuery = lowercase(query);
    if (lowerQuery.find("price") != std::string::npos ||
        lowerQuery.find("market") != std::string::npos ||
        lowerQuery.find("apr") != std::string::npos) {
        return "unsupported: no verified live-market snapshot was requested from an external adapter";
    }
    if (lowerQuery.find("pool") != std::string::npos &&
        lowerQuery.find("create") != std::string::npos) {
        return "Local pool bookkeeping is available; it does not create an on-chain wallet or liquidity pool.";
    }
    if (lowerQuery.find("liquidity") != std::string::npos) {
        return "unsupported: liquidity changes require a connected adapter and explicit confirmation; no APR is assumed";
    }
    if (lowerQuery.find("copy") != std::string::npos &&
        lowerQuery.find("trad") != std::string::npos) {
        return "unsupported: copy trading requires a connected adapter, validated limits, and explicit confirmation";
    }
    return "Local Spartan bookkeeping is ready. External market and trading actions require verified adapter evidence.";
}

std::string SpartanAgent::formatTokenPrice(const TokenInfo& token) const {
    if (!token.verified || !isValidToken(token.symbol) ||
        !isValidSolanaAddress(token.address) || !isFinitePositive(token.price) ||
        !std::isfinite(token.marketCap) || token.marketCap < 0.0 ||
        !std::isfinite(token.volume24h) || token.volume24h < 0.0 ||
        !std::isfinite(token.change24h) || token.asOfEpochSeconds <= 0 ||
        trim(token.dataSource).empty() || trim(token.evidenceReference).empty()) {
        return "unsupported: no verified market snapshot is available";
    }

    std::ostringstream stream;
    stream << std::setprecision(12) << "Verified " << token.symbol << " snapshot: $"
           << token.price << " | 24h change: "
           << (token.change24h >= 0.0 ? "+" : "") << token.change24h
           << "% | volume: $" << token.volume24h << " | market cap: $"
           << token.marketCap << " | observed_at_epoch: " << token.asOfEpochSeconds
           << " | source: " << token.dataSource
           << " | evidence: " << token.evidenceReference;
    const std::string output = stream.str();
    return hasForbiddenMarketClaim(output)
        ? "failed: adapter response contained an unqualified current-market claim"
        : output;
}

bool SpartanAgent::requiresConfirmation(const std::string& action) const {
    const std::string lowerAction = lowercase(action);
    const bool externalAction = lowerAction.find("swap") != std::string::npos ||
        lowerAction.find("trade") != std::string::npos ||
        lowerAction.find("liquidity") != std::string::npos;
    if (!externalAction) {
        return false;
    }
    std::lock_guard<std::mutex> lock(stateMutex_);
    return config_.requireConfirmation;
}

SpartanConfig SpartanAgent::sanitizeConfig(const SpartanConfig& config) {
    SpartanConfig sanitized = config;
    sanitized.solanaRpcUrl = trim(sanitized.solanaRpcUrl);
    sanitized.solanaPublicKey = trim(sanitized.solanaPublicKey);
    sanitized.solanaPrivateKey.clear();
    std::vector<std::string> dexes;
    dexes.reserve(sanitized.preferredDexes.size());
    for (const auto& dex : sanitized.preferredDexes) {
        dexes.push_back(trim(dex));
    }
    sanitized.preferredDexes = std::move(dexes);
    return sanitized;
}

bool SpartanAgent::validateConfigValue(const SpartanConfig& config) {
    if (!config.solanaRpcUrl.empty() && !isHttpUrl(config.solanaRpcUrl)) {
        return false;
    }
    if (!config.solanaPublicKey.empty() && !isValidSolanaAddress(config.solanaPublicKey)) {
        return false;
    }
    if (!isFinitePositive(config.maxTradeAmount) ||
        !std::isfinite(config.maxRiskPercentage) || config.maxRiskPercentage <= 0.0 ||
        config.maxRiskPercentage > 1.0 || !std::isfinite(config.portfolioValue) ||
        config.portfolioValue < 0.0 || config.preferredDexes.empty()) {
        return false;
    }
    std::set<std::string> normalizedDexes;
    for (const auto& dex : config.preferredDexes) {
        if (!isValidDexName(dex) || !normalizedDexes.insert(lowercase(dex)).second) {
            return false;
        }
    }
    return true;
}

bool SpartanAgent::isValidSolanaAddress(const std::string& value) {
    return value.size() >= kSolanaAddressMinLength && value.size() <= kSolanaAddressMaxLength &&
        std::all_of(value.begin(), value.end(), isBase58Character);
}

bool SpartanAgent::isValidIdentifier(const std::string& value) {
    return !value.empty() && value.size() <= kMaxIdentifierLength &&
        std::all_of(value.begin(), value.end(), [](unsigned char c) {
            return isAllowedIdentifierCharacter(c);
        });
}

bool SpartanAgent::isValidToken(const std::string& value) {
    return !value.empty() && value.size() <= 32 &&
        std::all_of(value.begin(), value.end(), [](unsigned char c) {
            return isAllowedTokenCharacter(c);
        });
}

bool SpartanAgent::isValidDexName(const std::string& value) {
    return isValidIdentifier(value);
}

bool SpartanAgent::isValidPoolId(const std::string& value) {
    return value.size() == 21 && value.rfind("pool_", 0) == 0 &&
        std::all_of(value.begin() + 5, value.end(), [](unsigned char c) {
            return std::isxdigit(c) != 0;
        });
}

bool SpartanAgent::isFinitePositive(double value) {
    return std::isfinite(value) && value > 0.0;
}

bool SpartanAgent::isEvidenceValid(const AdapterEvidence& evidence,
                                    bool requireVerified) {
    if (requireVerified && !evidence.verified) {
        return false;
    }
    return !trim(evidence.provider).empty() && !trim(evidence.reference).empty() &&
        evidence.observedAtEpochSeconds > 0;
}

std::string SpartanAgent::statusMessage(OperationStatus status,
                                         const std::string& detail) {
    return std::string(operationStatusName(status)) + ": " + detail;
}

std::string SpartanAgent::selectDexLocked(const std::string& dex) const {
    const std::string requested = trim(dex);
    if (requested.empty()) {
        return config_.preferredDexes.empty() ? std::string() : config_.preferredDexes.front();
    }
    if (!isValidDexName(requested)) {
        return {};
    }
    const std::string lowerRequested = lowercase(requested);
    const auto it = std::find_if(config_.preferredDexes.begin(), config_.preferredDexes.end(),
                                 [&](const std::string& candidate) {
                                     return lowercase(candidate) == lowerRequested;
                                 });
    return it == config_.preferredDexes.end() ? std::string() : *it;
}

bool SpartanAgent::validateTradeLocked(const std::string& fromToken,
                                        const std::string& toToken,
                                        double amount) const {
    const std::string from = trim(fromToken);
    const std::string to = trim(toToken);
    if (!initialized_ || !isValidToken(from) || !isValidToken(to) || from == to ||
        !isFinitePositive(amount) || amount > config_.maxTradeAmount) {
        return false;
    }
    return config_.portfolioValue <= 0.0 ||
        calculateRiskPercentage(amount, config_.portfolioValue) <= config_.maxRiskPercentage;
}

SpartanAgent::ExternalSnapshot SpartanAgent::externalSnapshotLocked() const {
    ExternalSnapshot snapshot;
    snapshot.adapter = adapter_;
    snapshot.generation = generation_;
    if (!initialized_) {
        snapshot.failureStatus = OperationStatus::Rejected;
        snapshot.failureMessage = "agent is not initialized";
    } else if (!adapter_) {
        snapshot.failureStatus = OperationStatus::Unsupported;
        snapshot.failureMessage = "no external adapter is installed; no action was performed";
    } else if (!externallyConnected_) {
        snapshot.failureStatus = OperationStatus::Rejected;
        snapshot.failureMessage = "external connection is not verified; no action was performed";
    } else {
        snapshot.ready = true;
    }
    return snapshot;
}

bool SpartanAgent::generationStillCurrent(const ExternalSnapshot& snapshot) const {
    return initialized_ && externallyConnected_ && generation_ == snapshot.generation &&
        adapter_ == snapshot.adapter;
}

std::string SpartanAgent::generatePoolIdLocked() {
    std::ostringstream stream;
    stream << "pool_" << std::hex << std::setw(16) << std::setfill('0') << nextPoolId_++;
    return stream.str();
}

void SpartanAgent::updateTokenCacheLocked(const TokenInfo& token) {
    tokenCache_[token.address] = token;
}

TokenQueryResult SpartanAgent::normalizeTokenResult(TokenQueryResult result,
                                                     const std::string& expected,
                                                     bool byAddress,
                                                     std::size_t maxCount,
                                                     const ExternalSnapshot& snapshot) {
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!generationStillCurrent(snapshot)) {
            return makeTokenResult(OperationStatus::Failed,
                                   "agent lifecycle changed during token query");
        }
    }
    if (result.status != OperationStatus::Verified) {
        result.token = {};
        result.tokens.clear();
        if (result.message.empty()) {
            result.message = statusMessage(result.status, "token query was not verified");
        } else if (lowercase(result.message).rfind(operationStatusName(result.status), 0) != 0) {
            result.message = statusMessage(result.status, result.message);
        }
        return result;
    }
    if (!isEvidenceValid(result.evidence, true)) {
        return makeTokenResult(OperationStatus::Failed,
                               "adapter claimed verified token data without valid evidence");
    }

    auto normalizeToken = [&](TokenInfo& token) {
        token.symbol = trim(token.symbol);
        token.address = trim(token.address);
        if (!isValidToken(token.symbol) || !isValidSolanaAddress(token.address) ||
            !isFinitePositive(token.price) || !std::isfinite(token.marketCap) ||
            token.marketCap < 0.0 || !std::isfinite(token.volume24h) ||
            token.volume24h < 0.0 || !std::isfinite(token.change24h)) {
            return false;
        }
        token.verified = true;
        token.asOfEpochSeconds = result.evidence.observedAtEpochSeconds;
        token.dataSource = result.evidence.provider;
        token.evidenceReference = result.evidence.reference;
        return true;
    };

    if (!expected.empty()) {
        if (!normalizeToken(result.token) ||
            (byAddress ? result.token.address != expected
                       : lowercase(result.token.symbol) != lowercase(expected))) {
            return makeTokenResult(OperationStatus::Failed,
                                   "adapter token response did not match the request");
        }
    } else {
        if (result.tokens.empty() || result.tokens.size() > maxCount) {
            return makeTokenResult(OperationStatus::Failed,
                                   "adapter returned an invalid top-token result count");
        }
        std::set<std::string> addresses;
        for (auto& token : result.tokens) {
            if (!normalizeToken(token) || !addresses.insert(token.address).second) {
                return makeTokenResult(OperationStatus::Failed,
                                       "adapter returned malformed or duplicate token data");
            }
        }
    }

    result.message = statusMessage(OperationStatus::Verified,
                                   result.message.empty() ? "token data verified by adapter"
                                                          : result.message);
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!generationStillCurrent(snapshot)) {
            return makeTokenResult(OperationStatus::Failed,
                                   "agent lifecycle changed before token data could be recorded");
        }
        if (!expected.empty()) {
            updateTokenCacheLocked(result.token);
        } else {
            for (const auto& token : result.tokens) {
                updateTokenCacheLocked(token);
            }
        }
    }
    return result;
}

TradeResult SpartanAgent::normalizeTradeResult(TradeResult result,
                                                double requestedAmount,
                                                const ExternalSnapshot& snapshot) {
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!generationStillCurrent(snapshot)) {
            return makeTradeResult(OperationStatus::Failed,
                                   "agent lifecycle changed during external action");
        }
    }
    result.success = false;
    if (result.status == OperationStatus::Verified) {
        if (!isEvidenceValid(result.evidence, true) || trim(result.transactionId).empty() ||
            !isFinitePositive(result.amountTraded) ||
            result.amountTraded > requestedAmount + 1e-12 ||
            !isFinitePositive(result.executionPrice)) {
            return makeTradeResult(OperationStatus::Failed,
                                   "adapter claimed a verified action without a valid receipt");
        }
        result.success = true;
    } else if (result.status == OperationStatus::Submitted) {
        if (trim(result.transactionId).empty() || !isEvidenceValid(result.evidence, false)) {
            return makeTradeResult(OperationStatus::Failed,
                                   "adapter submitted an action without submission evidence");
        }
        result.amountTraded = 0.0;
        result.executionPrice = 0.0;
    } else {
        result.transactionId.clear();
        result.amountTraded = 0.0;
        result.executionPrice = 0.0;
    }
    result.message = statusMessage(result.status,
                                   result.message.empty() ? "adapter result received"
                                                          : result.message);
    return result;
}

OperationResult SpartanAgent::normalizeOperationResult(
    OperationResult result, const ExternalSnapshot& snapshot) {
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (!generationStillCurrent(snapshot)) {
            return makeOperationResult(OperationStatus::Failed,
                                       "agent lifecycle changed during external action");
        }
    }
    if (result.status == OperationStatus::Verified) {
        if (!isEvidenceValid(result.evidence, true)) {
            return makeOperationResult(OperationStatus::Failed,
                                       "adapter claimed a verified action without valid evidence");
        }
    } else if (result.status == OperationStatus::Submitted) {
        if (!isEvidenceValid(result.evidence, false)) {
            return makeOperationResult(OperationStatus::Failed,
                                       "adapter submitted an action without evidence");
        }
    }
    result.message = statusMessage(result.status,
                                   result.message.empty() ? "adapter result received"
                                                          : result.message);
    return result;
}

std::unique_ptr<SpartanAgent> createSpartanAgent(
    const SpartanConfig& config,
    std::shared_ptr<ISpartanAdapter> adapter) {
    return std::make_unique<SpartanAgent>(config, std::move(adapter));
}

SpartanConfig getDefaultSpartanConfig() {
    SpartanConfig config;
    // Local-only by default. Installing an adapter and explicitly configuring an
    // endpoint are separate caller decisions.
    config.solanaRpcUrl.clear();
    config.solanaPublicKey.clear();
    config.solanaPrivateKey.clear();
    config.preferredDexes = {"Orca", "Raydium", "Meteora"};
    config.maxTradeAmount = 1000.0;
    config.maxRiskPercentage = 0.1;
    config.portfolioValue = 0.0;
    config.requireConfirmation = true;
    return config;
}

SpartanConfig getSpartanConfigFromEnvironment() {
    SpartanConfig config = getDefaultSpartanConfig();
    if (const char* rpcUrl = std::getenv("SOLANA_RPC_URL")) {
        config.solanaRpcUrl = rpcUrl;
    }
    if (const char* publicKey = std::getenv("SOLANA_PUBLIC_KEY")) {
        config.solanaPublicKey = publicKey;
    }
    // Private keys are intentionally not read from the process environment.
    config.solanaPrivateKey.clear();
    return config;
}

} // namespace elizaos
