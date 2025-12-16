#include ".target/types/raydium_vault.hpp"
#include "utils.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;
;
;

describe("raydium_vault", () => {
    // Configure the client to use the local cluster.
    const provider = anchor.AnchorProvider.env();
    anchor.setProvider(provider);
    const connection = provider.connection;

    const nodeWallet = provider.wallet as NodeWallet;
    const signerWallet = anchor.web3.Keypair.fromSecretKey(
        nodeWallet.payer.secretKey
    );

    const program = anchor.workspace.RaydiumVault as Program<RaydiumVault>;

    it("Deposit Position", async function () {
        const isDev = isDevnet(connection);
        const position_nft = getNftAddress(isDev);

        const [vault_config] = anchor.web3.PublicKey.findProgramAddressSync(
            [Buffer.from(vaultConfigSeed)],
            program.programId
        );
        const [user_position] = anchor.web3.PublicKey.findProgramAddressSync(
            [Buffer.from(positionSeed), position_nft.toBuffer()],
            program.programId
        );
        const from_account = spl.getAssociatedTokenAddressSync(
            position_nft,
            signerWallet.publicKey
        );
        const [nft_token_faucet] = anchor.web3.PublicKey.findProgramAddressSync(
            [Buffer.from(nftFaucetSeed), position_nft.toBuffer()],
            program.programId
        );
        const claimer_address = signerWallet.publicKey;

        await program.rpc.deposit(claimer_address, {
            accounts: {
                authority: signerWallet.publicKey,
                vaultConfig: vault_config,
                userPosition: user_position,
                positionNft: position_nft,
                fromAccount: from_account,
                nftTokenFaucet: nft_token_faucet,
                tokenProgram: spl.TOKEN_PROGRAM_ID,
                systemProgram: anchor.web3.SystemProgram.programId,
            },
        });
    });
});

} // namespace elizaos
