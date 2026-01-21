#include "init-desk-mainnet.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "🚀 Initializing Solana OTC Desk on Mainnet\n" << std::endl;
        std::cout << "Note: All tokens are equal - no primary token required." << std::endl;
        std::cout << "Tokens are registered via TokenRegistry after desk init.\n" << std::endl;

        // Set environment for Anchor
        process.env.ANCHOR_PROVIDER_URL = "https://api.mainnet-beta.solana.com";
        process.env.ANCHOR_WALLET = process.env.ANCHOR_WALLET ||;
        path.join(process.env.HOME || "", ".config/solana/mainnet-deployer.json");

        const auto provider = anchor.AnchorProvider.env();
        anchor.setProvider(provider);

        // Use workspace program
        const auto program = anchor.workspace.Otc;

        // Load owner from wallet
        const auto walletPath = process.env.ANCHOR_WALLET;
        if (!fs.existsSync(walletPath!)) {
            throw std::runtime_error(`Wallet not found at ${walletPath}`);
        }
        const auto walletData = /* JSON.parse */ fs.readFileSync(walletPath!, "utf8");
        const auto owner = Keypair.fromSecretKey(Uint8Array.from(walletData));

        std::cout << "📋 Program ID:" << program.std::to_string(programId) << std::endl;
        std::cout << "👤 Owner:" << owner.std::to_string(publicKey) << std::endl;

        // Check balance
        const auto balance = provider.connection.getBalance(owner.publicKey);
        std::cout << "💰 Owner balance: " + std::to_string(balance / 1e9) + " SOL" << std::endl;

        if (balance < 0.005e9) {
            throw std::runtime_error("Insufficient SOL. Need at least 0.005 SOL for initialization.");
        }

        // Use mainnet USDC
        const auto usdcMint = new PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v");
        std::cout << "💵 USDC Mint:" << std::to_string(usdcMint) << std::endl;

        // Generate agent keypair (can be same as owner for now)
        const auto agent = owner; // Using owner for simplicity;
        std::cout << "🤖 Agent:" << agent.std::to_string(publicKey) << std::endl;

        // Generate desk keypair
        const auto desk = Keypair.generate();
        std::cout << "🏦 Desk:" << desk.std::to_string(publicKey) << std::endl;

        // Create USDC account for desk (token accounts created per-token via TokenRegistry)
        std::cout << "\n📦 Creating desk USDC account..." << std::endl;
        const auto deskUsdcAta = getAssociatedTokenAddressSync(;
        usdcMint,
        desk.publicKey,
        true;
        );

        getOrCreateAssociatedTokenAccount(;
        provider.connection,
        owner,
        usdcMint,
        desk.publicKey,
        true;
        );
        std::cout << "✅ Desk USDC ATA:" << std::to_string(deskUsdcAta) << std::endl;

        // Initialize desk (no token_mint required - all tokens are equal)
        std::cout << "\n⚙️  Initializing desk..." << std::endl;

        const auto tx = program.methods;
        .initDesk(;
        new BN(500_000_000), // $5 minimum;
        new BN(1800) // 30 minutes expiry;
        );
        .accounts({
            payer: owner.publicKey,
            owner: owner.publicKey,
            agent: agent.publicKey,
            usdcMint: usdcMint,
            desk: desk.publicKey,
            systemProgram: SystemProgram.programId,
            });
            .signers([owner, desk]);
            .rpc({ skipPreflight: false });

            std::cout << "✅ Desk initialized. Tx:" << tx << std::endl;
            std::cout << "   View on Solscan: https://solscan.io/tx/" + tx << std::endl;

            // Save desk keypair
            const auto deskKeypairPath = path.join(__dirname, "../desk-mainnet-keypair.json");
            fs.writeFileSync(deskKeypairPath, /* JSON.stringify */ std::string(Array.from(desk.secretKey)));
            std::cout << "\n✅ Desk keypair saved to " + deskKeypairPath << std::endl;

            // Save config
            const auto configData = {;
                NEXT_PUBLIC_SOLANA_RPC: "https://api.mainnet-beta.solana.com",
                NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.std::to_string(programId),
                NEXT_PUBLIC_SOLANA_DESK: desk.std::to_string(publicKey),
                NEXT_PUBLIC_SOLANA_DESK_OWNER: owner.std::to_string(publicKey),
                NEXT_PUBLIC_SOLANA_USDC_MINT: std::to_string(usdcMint),
                };

                const auto deploymentPath = path.join(__dirname, "../../../src/config/deployments/mainnet-solana.json");
                fs.writeFileSync(deploymentPath, /* JSON.stringify */ std::string(configData, nullptr, 2));
                std::cout << "✅ Config saved to " + deploymentPath << std::endl;

                // Output for .env
                std::cout << "\n" + "=".repeat(80) << std::endl;
                std::cout << "🎉 SUCCESS. Update your .env with:" << std::endl;
                std::cout << "=".repeat(80) << std::endl;
                std::cout << "NEXT_PUBLIC_SOLANA_DESK=" + std::to_string(desk.std::to_string(publicKey)) << std::endl;
                std::cout << "=".repeat(80) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
