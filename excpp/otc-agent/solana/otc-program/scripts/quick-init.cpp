#include "quick-init.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🚀 Quick Solana OTC Desk Setup (LOCAL TESTING ONLY)\n" << std::endl;
    std::cout << "⚠️  This script uses setManualTokenPrice which requires the 'testing' feature." << std::endl;
    std::cout << "⚠️  Build with: anchor build -- --features testing" << std::endl;
    std::cout << "⚠️  Production uses Pyth oracle or on-chain pool pricing instead.\n" << std::endl;
    std::cout << "Note: All tokens are equal - registered via TokenRegistry.\n" << std::endl;

    const auto provider = AnchorProvider.env();
    setProvider(provider);
    const auto program = workspace.Otc<Otc>;

    std::cout << "📋 Program ID:" << program.programId.toString() << std::endl;

    // Load owner keypair
    const auto ownerData = JSON.parse(fs.readFileSync("./id.json", "utf8"));
    const auto owner = Keypair.fromSecretKey(Uint8Array.from(ownerData));
    std::cout << "👤 Owner:" << owner.publicKey.toString() << std::endl;

    // Check balance
    auto balance = provider.connection.getBalance(owner.publicKey);
    std::cout << "💰 Owner balance: " + std::to_string(balance / 1e9) + " SOL" << std::endl;

    if (balance < 3e9) {
        std::cout << "💸 Airdropping SOL..." << std::endl;
        const auto sig = provider.connection.requestAirdrop(owner.publicKey, 5e9);
        provider.connection.confirmTransaction(sig, "confirmed");
        balance = provider.connection.getBalance(owner.publicKey);
        std::cout << "💰 New balance: " + std::to_string(balance / 1e9) + " SOL" << std::endl;
    }

    // Generate agent and desk keypairs
    const auto agent = Keypair.generate();
    const auto agentSig = provider.connection.requestAirdrop(;
    agent.publicKey,
    1e9;
    );
    provider.connection.confirmTransaction(agentSig, "confirmed");
    std::cout << "🤖 Agent:" << agent.publicKey.toString() << std::endl;

    const auto desk = Keypair.generate();
    std::cout << "🏦 Desk (keypair):" << desk.publicKey.toString() << std::endl;

    // Create token mints for testing
    std::cout << "\n🪙 Creating test token mints..." << std::endl;
    std::cout << "⚠️  NOTE: Using 9 decimals for native Solana token (standard)" << std::endl;

    const auto tokenMint = createMint(;
    provider.connection,
    owner,
    owner.publicKey,
    nullptr,
    9  // Standard Solana token decimals;
    );
    std::cout << "✅ Token Mint:" << tokenMint.toString() << std::endl;

    const auto usdcMint = createMint(;
    provider.connection,
    owner,
    owner.publicKey,
    nullptr,
    6;
    );
    std::cout << "✅ USDC Mint:" << usdcMint.toString() << std::endl;

    // Create token accounts for desk
    std::cout << "\n📦 Creating desk token accounts..." << std::endl;
    const auto deskTokenAta = getAssociatedTokenAddressSync(;
    tokenMint,
    desk.publicKey,
    true;
    );
    const auto deskUsdcAta = getAssociatedTokenAddressSync(;
    usdcMint,
    desk.publicKey,
    true;
    );

    getOrCreateAssociatedTokenAccount(;
    provider.connection,
    owner,
    tokenMint,
    desk.publicKey,
    true;
    );
    std::cout << "✅ Desk token ATA:" << deskTokenAta.toString() << std::endl;

    getOrCreateAssociatedTokenAccount(;
    provider.connection,
    owner,
    usdcMint,
    desk.publicKey,
    true;
    );
    std::cout << "✅ Desk USDC ATA:" << deskUsdcAta.toString() << std::endl;

    // Initialize desk (no token_mint required - all tokens are equal)
    std::cout << "\n⚙️  Initializing desk..." << std::endl;
    const auto tx = program.methods;
    .initDesk(;
    new BN(500_000_000),
    new BN(1800);
    );
    .accountsPartial({
        payer: owner.publicKey,
        owner: owner.publicKey,
        agent: agent.publicKey,
        usdcMint: usdcMint,
        desk: desk.publicKey,
        });
        .signers([owner, desk]);
        .rpc();

        std::cout << "✅ Desk initialized. Tx:" << tx << std::endl;

        // Register test token via TokenRegistry
        std::cout << "\n📝 Registering test token via TokenRegistry..." << std::endl;
        const auto [tokenRegistryPda] = PublicKey.findProgramAddressSync(;
        [Buffer.from("registry"), desk.publicKey.toBuffer(), tokenMint.toBuffer()],
        program.programId;
        );

        program.methods;
        .registerToken(;
        Array(32).fill(0), // No Pyth feed for test token;
        PublicKey.default, // No pool for test token;
        0                  // PoolType::None (0=None, 1=Raydium, 2=Orca, 3=PumpSwap)
        );
        .accountsPartial({
            desk: desk.publicKey,
            payer: owner.publicKey,
            tokenMint: tokenMint,
            tokenRegistry: tokenRegistryPda,
            });
            .signers([owner]);
            .rpc();
            std::cout << "✅ Token registered in TokenRegistry:" << tokenRegistryPda.toString() << std::endl;

            // Set manual price on token registry (for testing)
            std::cout << "\n💲 Setting manual price on token registry..." << std::endl;
            program.methods;
            .setManualTokenPrice(new BN(1_000_000_000)) // $10 per token (8 decimals);
            .accountsPartial({
                tokenRegistry: tokenRegistryPda,
                desk: desk.publicKey,
                owner: owner.publicKey,
                });
                .signers([owner]);
                .rpc();
                std::cout << "✅ Token price set" << std::endl;

                // Set SOL price on desk
                std::cout << "\n💲 Setting SOL price..." << std::endl;
                program.methods;
                .setPrices(;
                new BN(1_000_000_000),      // token price (deprecated, kept for compatibility);
                new BN(100_000_000_00),     // SOL price: $100 (8 decimals)
                new BN(0),                   // updated_at (ignored, uses clock);
                new BN(3600)                 // max age;
                );
                .accountsPartial({ desk: desk.publicKey, owner: owner.publicKey })
                .signers([owner]);
                .rpc();
                std::cout << "✅ SOL price set" << std::endl;

                // Add owner as approver
                std::cout << "\n👤 Adding owner..." << std::endl;
                program.methods;
                .setApprover(owner.publicKey, true);
                .accountsPartial({ desk: desk.publicKey, owner: owner.publicKey })
                .signers([owner]);
                .rpc();
                std::cout << "✅ Owner added" << std::endl;

                // Mint tokens to owner
                std::cout << "\n💎 Minting tokens..." << std::endl;
                const auto ownerTokenAta = getOrCreateAssociatedTokenAccount(;
                provider.connection,
                owner,
                tokenMint,
                owner.publicKey;
                );

                mintTo(;
                provider.connection,
                owner,
                tokenMint,
                ownerTokenAta.address,
                owner,
                1_000_000_000_000_000;
                );
                std::cout << "✅ Minted 1 << 000 << 000 tokens to owner" << std::endl;

                // Deposit to desk (now requires token_registry)
                std::cout << "\n📥 Depositing tokens to desk..." << std::endl;
                program.methods;
                .depositTokens(new BN("500000000000000"));
                .accountsPartial({
                    desk: desk.publicKey,
                    tokenRegistry: tokenRegistryPda,
                    owner: owner.publicKey,
                    ownerTokenAta: ownerTokenAta.address,
                    deskTokenTreasury: deskTokenAta,
                    });
                    .signers([owner]);
                    .rpc();
                    std::cout << "✅ Deposited 500 << 000 tokens" << std::endl;

                    // Save desk keypair
                    const auto deskKeypairPath = path.join(__dirname, "../desk-keypair.json");
                    fs.writeFileSync(deskKeypairPath, JSON.stringify(Array.from(desk.secretKey)));
                    std::cout << "\n💾 Saved desk keypair to:" << deskKeypairPath << std::endl;

                    // Output for .env (no TOKEN_MINT - all tokens are equal)
                    std::cout << "\n" + "=".repeat(80) << std::endl;
                    std::cout << "🎉 SUCCESS. Update your .env.local with these values:" << std::endl;
                    std::cout << "=".repeat(80) << std::endl;
                    std::cout << "NEXT_PUBLIC_SOLANA_RPC=http://127.0.0.1:8899" << std::endl;
                    std::cout << "NEXT_PUBLIC_SOLANA_PROGRAM_ID=" + std::to_string(program.programId.toString()) << std::endl;
                    std::cout << "NEXT_PUBLIC_SOLANA_DESK=" + std::to_string(desk.publicKey.toString()) << std::endl;
                    std::cout << "NEXT_PUBLIC_SOLANA_DESK_OWNER=" + std::to_string(owner.publicKey.toString()) << std::endl;
                    std::cout << "NEXT_PUBLIC_SOLANA_USDC_MINT=" + std::to_string(usdcMint.toString()) << std::endl;
                    std::cout << "=".repeat(80) << std::endl;
                    std::cout << "\nNote: Test token mint for local testing:" << tokenMint.toString() << std::endl;
                    std::cout << "=".repeat(80) << std::endl;

                    // Write to src config (no TOKEN_MINT)
                    const auto deploymentPath = path.join(__dirname, "../../../src/config/deployments/local-solana.json");
                    const auto deploymentDir = path.dirname(deploymentPath);
                    if (!fs.existsSync(deploymentDir)) {
                        fs.mkdirSync(deploymentDir, { recursive: true });
                    }

                    const auto envData = {;
                        NEXT_PUBLIC_SOLANA_RPC: "http://127.0.0.1:8899",
                        NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.programId.toString(),
                        NEXT_PUBLIC_SOLANA_DESK: desk.publicKey.toString(),
                        NEXT_PUBLIC_SOLANA_DESK_OWNER: owner.publicKey.toString(),
                        NEXT_PUBLIC_SOLANA_USDC_MINT: usdcMint.toString(),
                        };

                        fs.writeFileSync(;
                        deploymentPath,
                        JSON.stringify(envData, nullptr, 2);
                        );
                        std::cout << "\n✅ Config saved to " + std::to_string(deploymentPath) << std::endl;

                        // Update .env.local
                        const auto envLocalPath = path.join(__dirname, "../../../.env.local");
                        auto envContent = "";
                        if (fs.existsSync(envLocalPath)) {
                            envContent = fs.readFileSync(envLocalPath, "utf8");
                        }

                        // Update or add each env var (no TOKEN_MINT)
                        const auto envVars = {;
                            NEXT_PUBLIC_SOLANA_RPC: "http://127.0.0.1:8899",
                            NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.programId.toString(),
                            NEXT_PUBLIC_SOLANA_DESK: desk.publicKey.toString(),
                            NEXT_PUBLIC_SOLANA_DESK_OWNER: owner.publicKey.toString(),
                            NEXT_PUBLIC_SOLANA_USDC_MINT: usdcMint.toString(),
                            };

                            for (const int [key, value] of Object.entries(envVars)) {
                                const auto regex = "^" + std::to_string(key) + "=.*$";
                                if (regex.test(envContent)) {
                                    std::to_string(key) + "=" + std::to_string(value);
                                    } else {
                                        "\n" + std::to_string(key) + "=" + std::to_string(value);
                                    }
                                }

                                // Remove old TOKEN_MINT if present
                                envContent = envContent.replace(/^NEXT_PUBLIC_SOLANA_TOKEN_MINT=.*$/m, "");
                                envContent = envContent.replace(/\n\n+/g, "\n\n"); // Clean up extra newlines;

                                fs.writeFileSync(envLocalPath, envContent.trim() + "\n");
                                std::cout << "✅ Updated " + std::to_string(envLocalPath) << std::endl;

}

} // namespace elizaos
