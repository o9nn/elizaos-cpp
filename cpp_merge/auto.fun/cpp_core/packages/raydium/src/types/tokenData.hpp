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
    std: status;
    std: txId;
    std: updatedAt;
    std::optional<{> createPool;
    std: status;
    std: txId;
    std: updatedAt;
    std::optional<{> lockLP;
    std: status;
    std: txId;
    std: updatedAt;
    std::optional<{> sendNft;
    std: status;
    std: txId;
    std: updatedAt;
    std::optional<{> depositNft;
    std: status;
    std: txId;
    std: updatedAt;
    std::optional<{> finalize;
    std: status;
    std: txId;
    std: updatedAt;
    std::optional<bool> lock;
    std::optional<std:> lastStep;
};

struct WithdrawnAmountsData {
    double withdrawnSol;
    double withdrawnTokens;
};

struct PoolInfoData {
    std: id;
    std: lpMint;
    std: baseVault;
    std: quoteVault;
};

struct TokenData {
    std: id;
    std: name;
    std: ticker;
    std: url;
    std: image;
    std::optional<std:> twitter;
    std::optional<std:> telegram;
    std::optional<std:> farcaster;
    std::optional<std:> website;
    std::optional<std:> discord;
    std::optional<std:> description;
    std: mint;
    std: creator;
    std::optional<std:> nftMinted;
    std::optional<std:> lockId;
    std::optional<std:> lockedAmount;
    std::optional<Date> lockedAt;
    std::optional<Date> harvestedAt;
    std: status;
    Date createdAt;
    std: lastUpdated;
    std::optional<Date> completedAt;
    std::optional<Date> withdrawnAt;
    std::optional<Date> migratedAt;
    std::optional<std:> marketId;
    std::optional<std:> baseVault;
    std::optional<std:> quoteVault;
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
    std::optional<std:> txId;
    std::optional<MigrationData> migration;
    std::optional<WithdrawnAmountsData | ''> withdrawnAmounts;
    std::optional<PoolInfoData> poolInfo;
    std::optional<std:> lockLpTxId;
    std::optional<std:> tokenSupply;
    std::optional<double> tokenSupplyUiAmount;
    std::optional<double> tokenDecimals;
    std::optional<Date> lastSupplyUpdate;
};

struct TokenDBData {
    std::optional<std:> id;
    std::optional<std:> name;
    std::optional<std:> ticker;
    std::optional<std:> url;
    std::optional<std:> image;
    std::optional<std:> twitter;
    std::optional<std:> telegram;
    std::optional<std:> farcaster;
    std::optional<std:> website;
    std::optional<std:> discord;
    std::optional<std:> description;
    std: mint;
    std::optional<std:> creator;
    std::optional<std:> nftMinted;
    std::optional<std:> lockId;
    std::optional<std:> lockedAmount;
    std::optional<Date> lockedAt;
    std::optional<Date> harvestedAt;
    std::optional<std:> status;
    std::optional<Date> createdAt;
    Date lastUpdated;
    std::optional<Date> completedAt;
    std::optional<Date> withdrawnAt;
    std::optional<Date> migratedAt;
    std::optional<std:> marketId;
    std::optional<std:> baseVault;
    std::optional<std:> quoteVault;
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
    std::optional<std:> txId;
    std::optional<std:> migration;
    std::optional<std:> withdrawnAmounts;
    std::optional<std:> poolInfo;
    std::optional<std:> lockLpTxId;
};


} // namespace elizaos
