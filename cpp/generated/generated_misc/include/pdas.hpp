#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_PDAS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_PDAS_H
#include "core.h"
#include "@coral-xyz/anchor.h"

extern std::any LOCKING_PROGRAM;
extern std::string LOCK_CP_AUTH_SEED;
extern std::string VAULT_CONFIG_SEED;
extern std::string POSITION_SEED;
extern std::string NFT_FAUCET_SEED;
extern std::string LOCKED_CP_LIQUIDITY_SEED;
std::shared_ptr<anchor::web3::PublicKey> getVaultConfig(std::shared_ptr<anchor::web3::PublicKey> programId);

std::shared_ptr<anchor::web3::PublicKey> getUserPosition(std::shared_ptr<anchor::web3::PublicKey> programId, std::shared_ptr<anchor::web3::PublicKey> positionNft);

std::shared_ptr<anchor::web3::PublicKey> getNftTokenFaucet(std::shared_ptr<anchor::web3::PublicKey> programId, std::shared_ptr<anchor::web3::PublicKey> positionNft);

std::shared_ptr<anchor::web3::PublicKey> getLockedLiquidity(std::shared_ptr<anchor::web3::PublicKey> positionNft, std::shared_ptr<anchor::web3::PublicKey> lockingProgram = LOCKING_PROGRAM);

#endif
