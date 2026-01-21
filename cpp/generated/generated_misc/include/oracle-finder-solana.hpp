#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_ORACLE-FINDER-SOLANA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_ORACLE-FINDER-SOLANA_H
#include "core.h"

class SolanaOracleInfo;

class SolanaOracleInfo : public object, public std::enable_shared_from_this<SolanaOracleInfo> {
public:
    using std::enable_shared_from_this<SolanaOracleInfo>::shared_from_this;
    std::any type;

    std::string address;

    std::string feedId;

    std::string poolAddress;

    double liquidity;

    boolean valid;

    std::string warning;
};

extern std::string PYTH_PROGRAM_ID;
extern std::string JUPITER_PROGRAM_ID;
std::shared_ptr<Promise<any>> findSolanaOracle(std::string tokenMint);

std::shared_ptr<Promise<any>> findPythFeed(std::string tokenMint);

std::shared_ptr<Promise<any>> findJupiterPool(std::string tokenMint);

std::shared_ptr<Promise<any>> findRaydiumPool(std::string tokenMint);

object validateSolanaOracle(std::shared_ptr<SolanaOracleInfo> oracle);

std::string formatOracleInfo(std::shared_ptr<SolanaOracleInfo> oracle);

object getSolanaRegistrationCost();

#endif
