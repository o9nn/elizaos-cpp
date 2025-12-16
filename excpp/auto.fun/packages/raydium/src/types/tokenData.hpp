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

struct MigrationData {
    std::optional<{> withdraw;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::optional<{> createPool;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::optional<{> lockLP;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::optional<{> sendNft;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::optional<{> depositNft;
    std::string status;
    std::string txId;
    std::string updatedAt;
    std::optional<{> finalize;
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
    std::optional<Date> lockedAt;
    std::optional<Date> harvestedAt;
    std::string status;
    Date createdAt;
    std::string lastUpdated;
    std::optional<Date> completedAt;
    std::optional<Date> withdrawnAt;
    std::optional<Date> migratedAt;
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
    std::optional<Date> lastVolumeReset;
    std::optional<Date> lastPriceUpdate;
    std::optional<double> holderCount;
    std::optional<std::string> txId;
    std::optional<MigrationData> migration;
    std::optional<WithdrawnAmountsData | ''> withdrawnAmounts;
    std::optional<PoolInfoData> poolInfo;
    std::optional<std::string> lockLpTxId;
    std::optional<std::string> tokenSupply;
    std::optional<double> tokenSupplyUiAmount;
    std::optional<double> tokenDecimals;
    std::optional<Date> lastSupplyUpdate;
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
    std::optional<Date> lockedAt;
    std::optional<Date> harvestedAt;
    std::optional<std::string> status;
    std::optional<Date> createdAt;
    Date lastUpdated;
    std::optional<Date> completedAt;
    std::optional<Date> withdrawnAt;
    std::optional<Date> migratedAt;
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
    std::optional<Date> lastVolumeReset;
    std::optional<Date> lastPriceUpdate;
    std::optional<double> holderCount;
    std::optional<std::string> txId;
    std::optional<std::string> migration;
    std::optional<std::string> withdrawnAmounts;
    std::optional<std::string> poolInfo;
    std::optional<std::string> lockLpTxId;
};


} // namespace elizaos
