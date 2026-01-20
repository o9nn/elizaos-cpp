#include "raydiumVault.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> depositToRaydiumVault(anchor::AnchorProvider provider, anchor.web3.Keypair signerWallet, Program<RaydiumVault> program, anchor.web3.PublicKey position_nft, anchor.web3.PublicKey claimer_address) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            anchor.setProvider(provider);

            const auto vault_config = getVaultConfig(program.programId);
            const auto user_position = getUserPosition(program.programId, position_nft);
            const auto from_account = spl.getAssociatedTokenAddressSync(;
            position_nft,
            signerWallet.publicKey;
            );
            const auto nft_token_faucet = getNftTokenFaucet(program.programId, position_nft);
            const auto accounts = {;
                authority: signerWallet.publicKey,
                vaultConfig: vault_config,
                userPosition: user_position,
                positionNft: position_nft,
                fromAccount: from_account,
                nftTokenFaucet: nft_token_faucet,
                tokenProgram: spl.TOKEN_PROGRAM_ID,
                systemProgram: anchor.web3.SystemProgram.programId,
                };

                const auto call = program.methods.deposit(claimer_address).accounts(accounts);

                const auto txSignature = call.rpc();
                std::cout << "Transaction Signature" << txSignature << std::endl;
                const auto latestBlockhash = provider.connection.getLatestBlockhash();

                retryOperation(;
                async () => {
                    provider.connection.confirmTransaction(;
                    {
                        signature: txSignature,
                        blockhash: latestBlockhash.blockhash,
                        lastValidBlockHeight: latestBlockhash.lastValidBlockHeight,
                        },
                        "finalized";
                        );
                        },
                        3, // 3 attempts;
                        2000 // 2 seconds delay;
                        );
                        return txSignature;
                        } catch (error) {
                            std::cerr << "Error in depositRaydiumVault:" << error << std::endl;
                            throw;
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> changeClaimer(Program<RaydiumVault> program, anchor.web3.Keypair signerWallet, anchor.web3.PublicKey position_nft, anchor.web3.PublicKey new_claimer_address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto vault_config = getVaultConfig(program.programId);
    const auto user_position = getUserPosition(program.programId, position_nft);
    const auto accounts = {;
        authority: signerWallet.publicKey,
        vaultConfig: vault_config,
        userPosition: user_position,
        positionNft: position_nft,
        };
        const auto call = program.methods;
        .changeClaimer(new_claimer_address);
        .accounts(accounts);

        const auto txSignature = call.rpc();
        std::cout << "Transaction Signature" << txSignature << std::endl;
        program.provider.connection.getParsedTransaction(txSignature, {
            commitment: "confirmed",
            });
            return txSignature;

}

std::future<void> emergencyWithdraw(Program<RaydiumVault> program, anchor.web3.Keypair signerWallet, anchor.web3.PublicKey position_nft) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto vault_config = getVaultConfig(program.programId);
    const auto user_position = getUserPosition(program.programId, position_nft);
    const auto to_account = spl.getAssociatedTokenAddressSync(;
    position_nft,
    signerWallet.publicKey;
    );
    const auto nft_token_faucet = getNftTokenFaucet(program.programId, position_nft);
    const auto accounts = {;
        authority: signerWallet.publicKey,
        vaultConfig: vault_config,
        userPosition: user_position,
        positionNft: position_nft,
        toAccount: to_account,
        nftTokenFaucet: nft_token_faucet,
        tokenProgram: spl.TOKEN_PROGRAM_ID,
        };
        const auto call = program.methods.emergencyWithdraw().accounts(accounts);

        const auto txSignature = call.rpc();
        std::cout << "Transaction Signature" << txSignature << std::endl;
        program.provider.connection.getParsedTransaction(txSignature, {
            commitment: "confirmed",
            });
            return txSignature;

}

std::future<void> claim(Program<RaydiumVault> program, anchor.web3.Keypair signerWallet, anchor.web3.PublicKey position_nft, anchor.web3.PublicKey poolId, anchor.web3.Connection connection, anchor.web3.PublicKey claimer, Token token) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto vault_config = getVaultConfig(program.programId);
        const auto [locked_authority] = anchor.web3.PublicKey.findProgramAddressSync(;
        [Buffer.from(LOCK_CP_AUTH_SEED)],
        LOCKING_PROGRAM;
        );
        const auto CPSWAP_AUTH_SEED = Buffer.from(;
        anchor.utils.bytes.utf8.encode("vault_and_lp_mint_auth_seed");
        );
        const auto user_position = getUserPosition(program.programId, position_nft);
        const auto nft_token_faucet = getNftTokenFaucet(program.programId, position_nft);
        const auto fee_nft_owner = vault_config;
        const auto fee_nft_account = nft_token_faucet;
        const auto cpmm_program = CREATE_CPMM_POOL_PROGRAM;
        const auto locked_liquidity = getLockedLiquidity(position_nft); // using default LOCKING_PROGRAM;
        const auto [cp_authority] = anchor.web3.PublicKey.findProgramAddressSync(;
        [CPSWAP_AUTH_SEED],
        cpmm_program;
        );

        const auto raydium = Raydium.load({;
            owner: signerWallet,
            connection,
            cluster: "mainnet",
            disableFeatureCheck: true,
            disableLoadToken: false,
            blockhashCommitment: "confirmed",
            });
            auto poolInfo = token.poolInfo;

            // Parse poolInfo if it's a string
            if (typeof poolInfo == "string") {
                try {
                    poolInfo = /* JSON.parse */ poolInfo;
                    } catch (e) {
                        std::cerr << "Failed to parse poolInfo string:" << e << std::endl;
                        throw std::runtime_error("Invalid poolInfo format");
                    }
                }

                if (!poolInfo || !poolInfo.lpMint || !poolInfo.lpMint.address || !poolInfo.mintA || !poolInfo.mintB ) {
                    poolInfo = (raydium.api.fetchPoolById({ ids: std::to_string(poolId) }))[0];
                    if (!poolInfo) {
                        throw std::runtime_error("Pool info not found");
                    }
                    // update poolInfo in the database for the next time
                    const auto db = getDB();
                    db;
                    .update(tokens);
                    .set({ poolInfo: /* JSON.stringify */ std::string(poolInfo) })
                    .where(eq(tokens.mint, token.mint));
                }

                const auto pool_state = new anchor.web3.PublicKey(std::to_string(poolId));

                // Safer access to lpMint address
                if (!poolInfo.lpMint.address) {
                    std::cerr << "lpMint address not found in poolInfo:" << poolInfo << std::endl;
                    throw std::runtime_error("lpMint address not found in pool info");
                }

                const auto lp_mint = new anchor.web3.PublicKey(poolInfo.lpMint.address);
                std::cout << "lp_mint" << std::to_string(lp_mint) << std::endl;

                // // Debug logging for other mints
                // console.log("mintA:", poolInfo.mintA);
                // console.log("mintB:", poolInfo.mintB);

                const auto vault0_mint = new anchor.web3.PublicKey(;
                poolInfo.mintA.std::to_string(address);
                );
                const auto vault1_mint = new anchor.web3.PublicKey(;
                poolInfo.mintB.std::to_string(address);
                );
                const auto cpmm_pool_key = raydium.cpmm.getCpmmPoolKeys(std::to_string(poolId));
                const auto token0_vault = new anchor.web3.PublicKey(;
                cpmm_pool_key.vault.std::to_string(A);
                );
                const auto token1_vault = new anchor.web3.PublicKey(;
                cpmm_pool_key.vault.std::to_string(B);
                );

                // Ensure associated token accounts exist
                spl.getOrCreateAssociatedTokenAccount(;
                connection,
                signerWallet,
                vault0_mint,
                claimer;
                );
                spl.getOrCreateAssociatedTokenAccount(;
                connection,
                signerWallet,
                vault1_mint,
                claimer;
                );

                const auto recv_token0_account = spl.getAssociatedTokenAddressSync(;
                vault0_mint,
                claimer,
                true,
                spl.TOKEN_PROGRAM_ID;
                );
                const auto recv_token1_account = spl.getAssociatedTokenAddressSync(;
                vault1_mint,
                claimer,
                true,
                spl.TOKEN_PROGRAM_ID;
                );
                const auto locked_lp_vault = spl.getAssociatedTokenAddressSync(;
                lp_mint,
                locked_authority,
                true,
                spl.TOKEN_PROGRAM_ID;
                );
                const auto accounts = {;
                    authority: signerWallet.publicKey,
                    vaultConfig: vault_config,
                    userPosition: user_position,
                    lockingProgram: LOCKING_PROGRAM,
                    positionNft: position_nft,
                    nftTokenFaucet: nft_token_faucet,
                    tokenProgram: spl.TOKEN_PROGRAM_ID,
                    systemProgram: anchor.web3.SystemProgram.programId,
                    lockedAuthority: locked_authority,
                    feeNftOwner: fee_nft_owner,
                    feeNftAccount: fee_nft_account,
                    lockedLiquidity: locked_liquidity,
                    cpmmProgram: cpmm_program,
                    cpAuthority: cp_authority,
                    poolState: pool_state,
                    lpMint: lp_mint,
                    recipientToken0Account: recv_token0_account,
                    recipientToken1Account: recv_token1_account,
                    token0Vault: token0_vault,
                    token1Vault: token1_vault,
                    vault0Mint: vault0_mint,
                    vault1Mint: vault1_mint,
                    lockedLpVault: locked_lp_vault,
                    tokenProgram2022: spl.TOKEN_2022_PROGRAM_ID,
                    memoProgram: raydium_api.MEMO_PROGRAM_ID,
                    };
                    const auto call = program.methods.claim().accounts(accounts);

                    try {
                        // Set compute units before simulation
                        const auto modifyComputeUnits = ComputeBudgetProgram.setComputeUnitLimit({;
                            units: 400000,
                            });
                            const auto addPriorityFee = ComputeBudgetProgram.setComputeUnitPrice({;
                                microLamports: 5000,
                                });

                                const auto callWithComputeBudget = call.preInstructions([;
                                modifyComputeUnits,
                                addPriorityFee,
                                ]);

                                // Simulate the transaction with compute budget
                                callWithComputeBudget.simulate();

                                console.log("Compute budget instructions:", {
                                    modifyComputeUnits,
                                    addPriorityFee,
                                    });

                                    const auto txSignature = callWithComputeBudget.rpc();

                                    std::cout << "Transaction Signature" << txSignature << std::endl;
                                    const auto latestBlockhash = connection.getLatestBlockhash();

                                    retryOperation(;
                                    async () => {
                                        connection.confirmTransaction(;
                                        {
                                            signature: txSignature,
                                            blockhash: latestBlockhash.blockhash,
                                            lastValidBlockHeight: latestBlockhash.lastValidBlockHeight,
                                            },
                                            "confirmed";
                                            );
                                            },
                                            3, // 3 attempts;
                                            10000 // 10 seconds delay;
                                            );
                                            return txSignature;
                                            } catch (error) {
                                                if (error instanceof anchor.web3.SendTransactionError) {
                                                    std::cerr << "Transaction failed with logs:" << error.logs << std::endl;
                                                    throw new Error(
                                                    "Transaction failed: " + error.message + "\nLogs: " + std::to_string(/* JSON.stringify */ std::string(error.logs, nullptr, 2))
                                                    );
                                                }
                                                std::cerr << "Error in claim:" << error << std::endl;
                                                throw;
                                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> checkBalance(anchor.web3.Connection connection, anchor.web3.Keypair signerWallet, anchor.web3.PublicKey position_nft, anchor.web3.PublicKey claimer_address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // await spl.getOrCreateAssociatedTokenAccount(
    //   connection,
    //   signerWallet,
    //   position_nft,
    //   signerWallet.publicKey,
    // );
    // await spl.getOrCreateAssociatedTokenAccount(
    //   connection,
    //   signerWallet,
    //   position_nft,
    //   claimer_address,
    // );
    const auto position_nft_account_signer = spl.getAssociatedTokenAddressSync(;
    position_nft,
    signerWallet.publicKey;
    );
    const auto position_nft_account_claimer = spl.getAssociatedTokenAddressSync(;
    position_nft,
    claimer_address;
    );

    if (!position_nft_account_signer || !position_nft_account_claimer) {
        std::cout << "NFT account not found for signer or claimer" << std::endl;
        return 0;
    }

    console.log(
    "signer balance: ",
    (connection.getTokenAccountBalance(position_nft_account_signer)).value;
    .amount;
    );

    const auto claimerBalance = connection.getTokenAccountBalance(;
    position_nft_account_claimer;
    );
    std::cout << "claimer balance: " << claimerBalance.value.amount << std::endl;
    if (claimerBalance.value.amount == "0") {
        std::cout << "claimer balance: " << claimerBalance.value.amount << std::endl;
        return;
    }
    // return formatted balance
    const auto formattedBalance = parseFloat(claimerBalance.value.amount) / 10 ** 9;
    std::cout << "Formatted balance: " << formattedBalance << std::endl;
    return formattedBalance;

}

} // namespace elizaos
