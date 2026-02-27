#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SCRIPTS_SOLANA_ADMIN_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SCRIPTS_SOLANA_ADMIN_H
#include "core.hpp"
#include "dotenv.hpp"
// External dependency removed
// External dependency removed
// External dependency removed
#include "bs58.hpp"
// Using alias removed (invalid transpilation)
#include "fs.hpp"
#include "path.hpp"

extern string SOLANA_RPC;
extern any PROGRAM_ID;
extern any DESK;
extern double POOL_TYPE_NONE;
extern std::shared_ptr<Buffer> EMPTY_PYTH_FEED;
std::shared_ptr<Promise<std::shared_ptr<Connection>>> getConnection();

std::shared_ptr<Promise<std::shared_ptr<Keypair>>> getWallet();

std::shared_ptr<Promise<std::shared_ptr<anchor::Program>>> getProgram(std::shared_ptr<Connection> connection, std::shared_ptr<Keypair> wallet);

std::shared_ptr<Promise<void>> createTreasury(string tokenMintStr);

std::shared_ptr<Promise<void>> registerToken(string tokenMintStr, double priceUsd = undefined);

std::shared_ptr<Promise<void>> setPrice(string tokenMintStr, double priceUsd);

std::shared_ptr<Promise<void>> showStatus();

void printUsage();

std::shared_ptr<Promise<void>> main();

#endif
