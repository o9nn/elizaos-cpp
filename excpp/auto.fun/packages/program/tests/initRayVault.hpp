#include "utils.ts.hpp"
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

/// <reference types="mocha" />
;
;
import type { RaydiumVault } from "../target/types/raydium_vault";
;
;

describe("raydium_vault", () => {
    // Configure the client to use the local cluster.
    const provider = anchor.AnchorProvider.env();
    anchor.setProvider(provider);

    const nodeWallet = provider.wallet as NodeWallet;
    const signerWallet = anchor.web3.Keypair.fromSecretKey(
        nodeWallet.payer.secretKey
    );

    const program = anchor.workspace.RaydiumVault as Program<RaydiumVault>;

    it("Initialize Vault Config", async () => {
        let vaultConfig = {
            executorAuthority: new anchor.web3.PublicKey("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"),
            emergencyAuthority: new anchor.web3.PublicKey("iGkGbxitDUdFhWSewd9gX2QzwTqJQSoFP5pkwvj25YP"),
            managerAuthority: new anchor.web3.PublicKey("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"),
        };
        const [vault] = anchor.web3.PublicKey.findProgramAddressSync(
            [Buffer.from(vaultConfigSeed)],
            program.programId
        );

        let vaultConfigInfo: any;

        try {
            vaultConfigInfo = await program.account.vaultConfig.fetch(vault);
        } catch (error) {
            await program.rpc.initialize(vaultConfig, {
                accounts: {
                    payer: signerWallet.publicKey,
                    vaultConfig: vault,
                    systemProgram: anchor.web3.SystemProgram.programId,
                },
            });
            vaultConfigInfo = await program.account.vaultConfig.fetch(vault);
        }

        console.log("executor: ", vaultConfigInfo.executorAuthority.toString());
        console.log(
            "emergency: ",
            vaultConfigInfo.emergencyAuthority.toString()
        );
        console.log("manager: ", vaultConfigInfo.managerAuthority.toString());
    });
});

} // namespace elizaos
