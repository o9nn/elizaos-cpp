#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_RAYDIUMVAULT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_RAYDIUMVAULT_H
#include "core.h"
#include "@coral-xyz/anchor.h"
#include "@solana/spl-token.h"
#include "@coral-xyz/anchor.h"
#include "@raydium-io/raydium-sdk-v2.h"
#include "@raydium-io/raydium-sdk-v2.h"
#include "./pdas.h"
#include "@autodotfun/types/types/raydium_vault.h"
#include "./utils.h"
#include "@autodotfun/server/src/db.h"
#include "drizzle-orm.h"
#include "@solana/web3.js.h"

any depositToRaydiumVault(std::shared_ptr<anchor::AnchorProvider> provider, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> claimer_address);

any changeClaimer(std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> new_claimer_address);

any emergencyWithdraw(std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft);

any claim(std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> poolId, std::shared_ptr<anchor::web3::Connection> connection, std::shared_ptr<anchor::web3::PublicKey> claimer, std::shared_ptr<Token> token);

any checkBalance(std::shared_ptr<anchor::web3::Connection> connection, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> claimer_address);

#endif
