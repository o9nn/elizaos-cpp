#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct MigrationData {
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::optional<bool> lock;
    std::optional<std::string> lastStep;
};

struct WithdrawnAmountsData {
    double withdrawnSol;
    double withdrawnTokens;
};

struct PoolInfoData {
    std::string id;
    std::string lpMint;
    std::string baseVault;
    std::string quoteVault;
};

struct TokenData {
    std::string id;
    std::string name;
    std::string ticker;
    std::string url;
    std::string image;
    std::optional<std::string> twitter;
    std::optional<std::string> telegram;
    std::optional<std::string> farcaster;
    std::optional<std::string> website;
    std::optional<std::string> discord;
    std::optional<std::string> description;
    std::string mint;
    std::string creator;
    std::optional<std::string> nftMinted;
    std::optional<std::string> lockId;
    std::optional<std::string> lockedAmount;
    std::optional<std::chrono::system_clock::time_point> lockedAt;
    std::optional<std::chrono::system_clock::time_point> harvestedAt;
    std::string status;
    std::chrono::system_clock::time_point createdAt;
    std::string lastUpdated;
    std::optional<std::chrono::system_clock::time_point> completedAt;
    std::optional<std::chrono::system_clock::time_point> withdrawnAt;
    std::optional<std::chrono::system_clock::time_point> migratedAt;
    std::optional<std::string> marketId;
    std::optional<std::string> baseVault;
    std::optional<std::string> quoteVault;
    std::optional<double> withdrawnAmount;
    std::optional<double> reserveAmount;
    std::optional<double> reserveLamport;
    std::optional<double> virtualReserves;
    std::optional<double> liquidity;
    std::optional<double> currentPrice;
    std::optional<double> marketCapUSD;
    std::optional<double> tokenPriceUSD;
    std::optional<double> solPriceUSD;
    std::optional<double> curveProgress;
    std::optional<double> curveLimit;
    std::optional<double> priceChange24h;
    std::optional<double> price24hAgo;
    std::optional<double> volume24h;
    std::optional<double> inferenceCount;
    std::optional<std::chrono::system_clock::time_point> lastVolumeReset;
    std::optional<std::chrono::system_clock::time_point> lastPriceUpdate;
    std::optional<double> holderCount;
    std::optional<std::string> txId;
    std::optional<MigrationData> migration;
    std::optional<std::variant<WithdrawnAmountsData, std::string>> withdrawnAmounts;
    std::optional<PoolInfoData> poolInfo;
    std::optional<std::string> lockLpTxId;
    std::optional<std::string> tokenSupply;
    std::optional<double> tokenSupplyUiAmount;
    std::optional<double> tokenDecimals;
    std::optional<std::chrono::system_clock::time_point> lastSupplyUpdate;
};

struct TokenDBData {
    std::optional<std::string> id;
    std::optional<std::string> name;
    std::optional<std::string> ticker;
    std::optional<std::string> url;
    std::optional<std::string> image;
    std::optional<std::string> twitter;
    std::optional<std::string> telegram;
    std::optional<std::string> farcaster;
    std::optional<std::string> website;
    std::optional<std::string> discord;
    std::optional<std::string> description;
    std::string mint;
    std::optional<std::string> creator;
    std::optional<std::string> nftMinted;
    std::optional<std::string> lockId;
    std::optional<std::string> lockedAmount;
    std::optional<std::chrono::system_clock::time_point> lockedAt;
    std::optional<std::chrono::system_clock::time_point> harvestedAt;
    std::optional<std::string> status;
    std::optional<std::chrono::system_clock::time_point> createdAt;
    std::chrono::system_clock::time_point lastUpdated;
    std::optional<std::chrono::system_clock::time_point> completedAt;
    std::optional<std::chrono::system_clock::time_point> withdrawnAt;
    std::optional<std::chrono::system_clock::time_point> migratedAt;
    std::optional<std::string> marketId;
    std::optional<std::string> baseVault;
    std::optional<std::string> quoteVault;
    std::optional<double> withdrawnAmount;
    std::optional<double> reserveAmount;
    std::optional<double> reserveLamport;
    std::optional<double> virtualReserves;
    std::optional<double> liquidity;
    std::optional<double> currentPrice;
    std::optional<double> marketCapUSD;
    std::optional<double> tokenPriceUSD;
    std::optional<double> solPriceUSD;
    std::optional<double> curveProgress;
    std::optional<double> curveLimit;
    std::optional<double> priceChange24h;
    std::optional<double> price24hAgo;
    std::optional<double> volume24h;
    std::optional<double> inferenceCount;
    std::optional<std::chrono::system_clock::time_point> lastVolumeReset;
    std::optional<std::chrono::system_clock::time_point> lastPriceUpdate;
    std::optional<double> holderCount;
    std::optional<std::string> txId;
    std::optional<std::string> migration;
    std::optional<std::string> withdrawnAmounts;
    std::optional<std::string> poolInfo;
    std::optional<std::string> lockLpTxId;
};


} // namespace elizaos
