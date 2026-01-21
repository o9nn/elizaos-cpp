#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_CLAIMFEES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_CLAIMFEES_H
#include "core.h"
#include "@solana/web3.js.h"
#include "@autodotfun/raydium/src/raydiumVault.h"
#include "./tokenSupplyHelpers/customWallet.h"
#include "@coral-xyz/anchor.h"
#include "@autodotfun/types/types/raydium_vault.h"
#include "@autodotfun/types/idl/raydium_vault.json.h"
#include "./websocket-client.h"
#include "./db.h"

extern std::shared_ptr<RaydiumVault> raydium_vault_IDL;
std::shared_ptr<Promise<string>> claimFees(std::shared_ptr<PublicKey> nftMint, std::shared_ptr<PublicKey> poolId, std::shared_ptr<Connection> connection, std::shared_ptr<PublicKey> claimer, std::shared_ptr<WebSocketClient> websocket, std::shared_ptr<Token> token);

#endif
