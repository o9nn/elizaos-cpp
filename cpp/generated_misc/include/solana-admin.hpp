#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SCRIPTS_SOLANA-ADMIN_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SCRIPTS_SOLANA-ADMIN_H
#include "core.h"
#include "dotenv.h"
#include "@coral-xyz/anchor.h"
#include "@solana/web3.js.h"
#include "@solana/spl-token.h"
#include "bs58.h"
using bs58 = _default;
#include "fs.h"
#include "path.h"

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
