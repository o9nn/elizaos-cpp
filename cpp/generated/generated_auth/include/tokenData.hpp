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

    std::string lastStep;
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
    std::string id;

    std::string lpMint;

    std::string baseVault;

    std::string quoteVault;
};

class TokenData : public object, public std::enable_shared_from_this<TokenData> {
public:
    using std::enable_shared_from_this<TokenData>::shared_from_this;
    std::string id;

    std::string name;

    std::string ticker;

    std::string url;

    std::string image;

    std::string twitter;

    std::string telegram;

    std::string farcaster;

    std::string website;

    std::string discord;

    std::string description;

    std::string mint;

    std::string creator;

    std::string nftMinted;

    std::string lockId;

    std::string lockedAmount;

    std::shared_ptr<Date> lockedAt;

    std::shared_ptr<Date> harvestedAt;

    std::string status;

    std::shared_ptr<Date> createdAt;

    std::string lastUpdated;

    std::shared_ptr<Date> completedAt;

    std::shared_ptr<Date> withdrawnAt;

    std::shared_ptr<Date> migratedAt;

    std::string marketId;

    std::string baseVault;

    std::string quoteVault;

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

    std::string txId;

    std::shared_ptr<MigrationData> migration;

    std::any withdrawnAmounts;

    std::shared_ptr<PoolInfoData> poolInfo;

    std::string lockLpTxId;

    std::string tokenSupply;

    double tokenSupplyUiAmount;

    double tokenDecimals;

    std::shared_ptr<Date> lastSupplyUpdate;
};

class TokenDBData : public object, public std::enable_shared_from_this<TokenDBData> {
public:
    using std::enable_shared_from_this<TokenDBData>::shared_from_this;
    std::string id;

    std::string name;

    std::string ticker;

    std::string url;

    std::string image;

    std::string twitter;

    std::string telegram;

    std::string farcaster;

    std::string website;

    std::string discord;

    std::string description;

    std::string mint;

    std::string creator;

    std::string nftMinted;

    std::string lockId;

    std::string lockedAmount;

    std::shared_ptr<Date> lockedAt;

    std::shared_ptr<Date> harvestedAt;

    std::string status;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> lastUpdated;

    std::shared_ptr<Date> completedAt;

    std::shared_ptr<Date> withdrawnAt;

    std::shared_ptr<Date> migratedAt;

    std::string marketId;

    std::string baseVault;

    std::string quoteVault;

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

    std::string txId;

    std::string migration;

    std::string withdrawnAmounts;

    std::string poolInfo;

    std::string lockLpTxId;
};

#endif
