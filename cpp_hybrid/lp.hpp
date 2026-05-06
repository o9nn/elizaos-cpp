#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_LP_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_LP_H
#include "core.h"
#include "./primitives.h"
#include "./service.h"
#include "./token.h"

typedef object PoolInfo;

typedef object LpPositionDetails;

typedef object TransactionResult;

class ILpService;

class ILpService : public Service, public std::enable_shared_from_this<ILpService> {
public:
    using std::enable_shared_from_this<ILpService>::shared_from_this;
    static any override;

    string serviceType = std::string("lp");

    string capabilityDescription = std::string("Provides standardized access to DEX liquidity pools.");

    virtual string getDexName() = 0;
    virtual std::shared_ptr<Promise<array<PoolInfo>>> getPools(string tokenAMint = undefined, string tokenBMint = undefined) = 0;
    virtual std::shared_ptr<Promise<any>> addLiquidity(object params) = 0;
    virtual std::shared_ptr<Promise<any>> removeLiquidity(object params) = 0;
    virtual std::shared_ptr<Promise<any>> getLpPositionDetails(string userAccountPublicKey, string poolOrPositionIdentifier) = 0;
    virtual std::shared_ptr<Promise<Record<string, Partial<PoolInfo>>>> getMarketDataForPools(array<string> poolIds) = 0;
    ILpService(std::shared_ptr<IAgentRuntime> runtime = undefined);
};

#endif
