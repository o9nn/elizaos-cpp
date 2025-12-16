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

describe("raydium_vault", () => {
    // Configure the client to use the local cluster.
    const provider = anchor.AnchorProvider.env();
    anchor.setProvider(provider);
    const connection = provider.connection;

    const nodeWallet = provider.wallet as NodeWallet;
    const signerWallet = anchor.web3.Keypair.fromSecretKey(
        nodeWallet.payer.secretKey
    );

    it("Check Balance", async () => {
        const isDev = isDevnet(connection);
        const position_nft = getNftAddress(isDev);
        const claimer = claimer_address_0;

        await spl.getOrCreateAssociatedTokenAccount(
            connection,
            signerWallet,
            position_nft,
            signerWallet.publicKey
        );
        await spl.getOrCreateAssociatedTokenAccount(
            connection,
            signerWallet,
            position_nft,
            claimer
        );

        const position_nft_account_signer = spl.getAssociatedTokenAddressSync(
            position_nft,
            signerWallet.publicKey
        );
        const position_nft_account_claimer = spl.getAssociatedTokenAddressSync(
            position_nft,
            claimer
        );

        console.log(
            "signer balance: ",
            (
                await connection.getTokenAccountBalance(
                    position_nft_account_signer
                )
            ).value.amount
        );
        console.log(
            "claimer balance: ",
            (
                await connection.getTokenAccountBalance(
                    position_nft_account_claimer
                )
            ).value.amount
        );
    });
});

} // namespace elizaos
