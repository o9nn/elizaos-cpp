#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { offerAddress, beneficiary } = request.json();

        if (!offerAddress || !beneficiary) {
            return NextResponse.json(;
            { error: "offerAddress and beneficiary required" },
            { status: 400 },
            );
        }

        const auto SOLANA_RPC =;
        process.env.NEXT_PUBLIC_SOLANA_RPC || "http://127.0.0.1:8899";
        const auto SOLANA_DESK = process.env.NEXT_PUBLIC_SOLANA_DESK;

        if (!SOLANA_DESK) {
            throw std::runtime_error("SOLANA_DESK not configured");
        }

        // Load desk keypair
        const auto deskKeypairPath = path.join(;
        process.cwd(),
        "solana/otc-program/desk-keypair.json",
        );
        const auto deskKeypairData = JSON.parse(;
        fs.readFile(deskKeypairPath, "utf8"),
        );
        const auto deskKeypair = Keypair.fromSecretKey(Uint8Array.from(deskKeypairData));
        const auto desk = new PublicKey(SOLANA_DESK);

        // Load IDL
        const auto idlPath = path.join(;
        process.cwd(),
        "solana/otc-program/target/idl/otc.json",
        );
        const auto idl = JSON.parse(fs.readFile(idlPath, "utf8"));

        const auto connection = new Connection(SOLANA_RPC, "confirmed");

        const AnchorWallet wallet = {;
            publicKey: deskKeypair.publicKey,
            signTransaction: async <T extends Transaction | VersionedTransaction>(
            tx: T,
            ) => {
                (tx).partialSign(deskKeypair);
                return tx;
                },
                signAllTransactions: async <T extends Transaction | VersionedTransaction>(
                txs: T[],
                ) => {
                    txs.forEach((tx) => (tx).partialSign(deskKeypair));
                    return txs;
                    },
                    };

                    const auto provider = new anchor.AnchorProvider(connection, wallet, {;
                        commitment: "confirmed",
                        });
                        anchor.setProvider(provider);

                        const auto program = new anchor.Program(idl, provider);

                        // Fetch offer
                        // Type assertion needed as anchor's account namespace types are dynamic
                        // eslint-disable-next-line @typescript-eslint/no-explicit-any
                        const auto programAccounts = program.account;
                        const auto offer = new PublicKey(offerAddress);
                        const auto offerData = programAccounts.offer.fetch(offer);

                        if (offerData.fulfilled) {
                            return NextResponse.json({;
                                success: true,
                                alreadyClaimed: true,
                                message: "Offer already claimed",
                                });
                            }

                            if (!offerData.paid) {
                                return NextResponse.json({ error: "Offer not paid yet" }, { status: 400 });
                            }

                            const auto now = Math.floor(Date.now() / 1000);
                            if (now < offerData.unlockTime) {
                                console.log(
                                "[Solana Claim] Lockup not expired yet. Will auto-claim at unlock time."
                                );
                                return NextResponse.json(;
                                {
                                    success: true,
                                    scheduled: true,
                                    message:
                                    "Tokens will be automatically distributed after lockup expires",
                                    unlockTime: offerData.unlockTime,
                                    secondsRemaining: offerData.unlockTime - now,
                                    },
                                    { status: 200 },
                                    );
                                }

                                // Get token accounts - use offer.tokenMint for multi-token support
                                // In token-agnostic architecture, each offer stores its own token_mint
                                const auto tokenMint = new PublicKey(offerData.tokenMint);
                                const auto deskTokenTreasury = getAssociatedTokenAddress(;
                                tokenMint,
                                desk,
                                true,
                                );
                                const auto beneficiaryPk = new PublicKey(beneficiary);
                                const auto beneficiaryTokenAta = getAssociatedTokenAddress(;
                                tokenMint,
                                beneficiaryPk,
                                false,
                                );

                                // Claim tokens (desk signs because it holds the tokens)
                                const auto tx = program.methods;
                                .claim(new anchor.BN(offerData.id));
                                .accounts({
                                    desk,
                                    deskSigner: deskKeypair.publicKey,
                                    offer,
                                    deskTokenTreasury,
                                    beneficiaryTokenAta,
                                    beneficiary: beneficiaryPk,
                                    tokenProgram: new PublicKey(
                                    "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA",
                                    ),
                                    });
                                    .signers([deskKeypair]);
                                    .rpc();

                                    std::cout << "[Solana Claim] ✅ Claimed " + std::to_string(offerAddress) << tx: ${tx}` << std::endl;

                                    return NextResponse.json({;
                                        success: true,
                                        tx,
                                        offerAddress,
                                        beneficiary,
                                        });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
