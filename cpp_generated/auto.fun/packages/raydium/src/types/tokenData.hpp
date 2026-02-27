#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_TYPES_TOKENDATA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_TYPES_TOKENDATA_H
#include "core.h"

class MigrationData;
class WithdrawnAmountsData;
class PoolInfoData;
class TokenData;
class TokenDBData;

class MigrationData : public object, public std::enable_shared_from_this<MigrationData> {
public:
    using std::enable_shared_from_this<MigrationData>::shared_from_this;
    object withdraw;

    object createPool;

    object lockLP;

    object sendNft;

    object depositNft;

    object finalize;

    boolean lock;

    string lastStep;
};

class WithdrawnAmountsData : public object, public std::enable_shared_from_this<WithdrawnAmountsData> {
public:
    using std::enable_shared_from_this<WithdrawnAmountsData>::shared_from_this;
    double withdrawnSol;

    double withdrawnTokens;
};

class PoolInfoData : public object, public std::enable_shared_from_this<PoolInfoData> {
public:
    using std::enable_shared_from_this<PoolInfoData>::shared_from_this;
    string id;

    string lpMint;

    string baseVault;

    string quoteVault;
};

class TokenData : public object, public std::enable_shared_from_this<TokenData> {
public:
    using std::enable_shared_from_this<TokenData>::shared_from_this;
    string id;

    string name;

    string ticker;

    string url;

    string image;

    string twitter;

    string telegram;

    string farcaster;

    string website;

    string discord;

    string description;

    string mint;

    string creator;

    string nftMinted;

    string lockId;

    string lockedAmount;

    std::shared_ptr<Date> lockedAt;

    std::shared_ptr<Date> harvestedAt;

    string status;

    std::shared_ptr<Date> createdAt;

    string lastUpdated;

    std::shared_ptr<Date> completedAt;

    std::shared_ptr<Date> withdrawnAt;

    std::shared_ptr<Date> migratedAt;

    string marketId;

    string baseVault;

    string quoteVault;

    double withdrawnAmount;

    double reserveAmount;

    double reserveLamport;

    double virtualReserves;

    double liquidity;

    double currentPrice;

    double marketCapUSD;

    double tokenPriceUSD;

    double solPriceUSD;

    double curveProgress;

    double curveLimit;

    double priceChange24h;

    double price24hAgo;

    double volume24h;

    double inferenceCount;

    std::shared_ptr<Date> lastVolumeReset;

    std::shared_ptr<Date> lastPriceUpdate;

    double holderCount;

    string txId;

    std::shared_ptr<MigrationData> migration;

    any withdrawnAmounts;

    std::shared_ptr<PoolInfoData> poolInfo;

    string lockLpTxId;

    string tokenSupply;

    double tokenSupplyUiAmount;

    double tokenDecimals;

    std::shared_ptr<Date> lastSupplyUpdate;
};

class TokenDBData : public object, public std::enable_shared_from_this<TokenDBData> {
public:
    using std::enable_shared_from_this<TokenDBData>::shared_from_this;
    string id;

    string name;

    string ticker;

    string url;

    string image;

    string twitter;

    string telegram;

    string farcaster;

    string website;

    string discord;

    string description;

    string mint;

    string creator;

    string nftMinted;

    string lockId;

    string lockedAmount;

    std::shared_ptr<Date> lockedAt;

    std::shared_ptr<Date> harvestedAt;

    string status;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> lastUpdated;

    std::shared_ptr<Date> completedAt;

    std::shared_ptr<Date> withdrawnAt;

    std::shared_ptr<Date> migratedAt;

    string marketId;

    string baseVault;

    string quoteVault;

    double withdrawnAmount;

    double reserveAmount;

    double reserveLamport;

    double virtualReserves;

    double liquidity;

    double currentPrice;

    double marketCapUSD;

    double tokenPriceUSD;

    double solPriceUSD;

    double curveProgress;

    double curveLimit;

    double priceChange24h;

    double price24hAgo;

    double volume24h;

    double inferenceCount;

    std::shared_ptr<Date> lastVolumeReset;

    std::shared_ptr<Date> lastPriceUpdate;

    double holderCount;

    string txId;

    string migration;

    string withdrawnAmounts;

    string poolInfo;

    string lockLpTxId;
};

#endif
