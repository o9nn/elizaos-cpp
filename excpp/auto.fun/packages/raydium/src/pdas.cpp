#include "pdas.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

anchor.web3.PublicKey getVaultConfig(anchor.web3.PublicKey programId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return anchor.web3.PublicKey.findProgramAddressSync(;
    [Buffer.from(VAULT_CONFIG_SEED)],
    programId,
    )[0];

}

anchor.web3.PublicKey getUserPosition(anchor.web3.PublicKey programId, anchor.web3.PublicKey positionNft) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return anchor.web3.PublicKey.findProgramAddressSync(;
    [Buffer.from(POSITION_SEED), positionNft.toBuffer()],
    programId,
    )[0];

}

anchor.web3.PublicKey getNftTokenFaucet(anchor.web3.PublicKey programId, anchor.web3.PublicKey positionNft) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return anchor.web3.PublicKey.findProgramAddressSync(;
    [Buffer.from(NFT_FAUCET_SEED), positionNft.toBuffer()],
    programId,
    )[0];

}

anchor.web3.PublicKey getLockedLiquidity(anchor.web3.PublicKey positionNft, anchor.web3.PublicKey lockingProgram = LOCKING_PROGRAM) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return anchor.web3.PublicKey.findProgramAddressSync(;
    [Buffer.from(LOCKED_CP_LIQUIDITY_SEED), positionNft.toBuffer()],
    lockingProgram,
    )[0];

}

} // namespace elizaos
