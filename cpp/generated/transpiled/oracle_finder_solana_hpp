#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_ORACLE-FINDER-SOLANA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_ORACLE-FINDER-SOLANA_H
#include "core.h"

class SolanaOracleInfo;

class SolanaOracleInfo : public object, public std::enable_shared_from_this<SolanaOracleInfo> {
public:
    using std::enable_shared_from_this<SolanaOracleInfo>::shared_from_this;
    any type;

    string address;

    string feedId;

    string poolAddress;

    double liquidity;

    boolean valid;

    string warning;
};

extern string PYTH_PROGRAM_ID;
extern string JUPITER_PROGRAM_ID;
std::shared_ptr<Promise<any>> findSolanaOracle(string tokenMint);

std::shared_ptr<Promise<any>> findPythFeed(string tokenMint);

std::shared_ptr<Promise<any>> findJupiterPool(string tokenMint);

std::shared_ptr<Promise<any>> findRaydiumPool(string tokenMint);

object validateSolanaOracle(std::shared_ptr<SolanaOracleInfo> oracle);

string formatOracleInfo(std::shared_ptr<SolanaOracleInfo> oracle);

object getSolanaRegistrationCost();

#endif
