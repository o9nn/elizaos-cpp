#include "deploy-devnet.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🚀 Deploying Solana OTC Desk to Devnet\n" << std::endl;
    std::cout << "Note: All tokens are equal - no primary token required.\n" << std::endl;

    // Configure provider from env (ANCHOR_PROVIDER_URL, ANCHOR_WALLET)
    const auto provider = AnchorProvider.env();
    setProvider(provider);
    const auto program = workspace.Otc<Otc>;

    std::cout << "📋 Program ID:" << program.programId.toString() << std::endl;
    std::cout << "👤 Payer:" << provider.wallet.publicKey.toString() << std::endl;

    // 1. Load or Create Desk Keypair
    const auto deskKeypairPath = path.join(__dirname, "../desk-devnet-keypair.json");
    auto desk: Keypair;

    if (fs.existsSync(deskKeypairPath)) {
        const auto secret = JSON.parse(fs.readFileSync(deskKeypairPath, "utf8"));
        desk = Keypair.fromSecretKey(Uint8Array.from(secret));
        std::cout << "🏦 Using existing Desk:" << desk.publicKey.toString() << std::endl;
        } else {
            desk = Keypair.generate();
            fs.writeFileSync(deskKeypairPath, JSON.stringify(Array.from(desk.secretKey)));
            std::cout << "🏦 Created new Desk:" << desk.publicKey.toString() << std::endl;
        }

        // 2. Create USDC Mock Mint for Devnet testing
        std::cout << "\n🪙 Creating Devnet USDC Mock..." << std::endl;
        const auto payer = (provider.wallet).payer || Keypair.fromSecretKey(Buffer.from(JSON.parse(fs.readFileSync(process.env.ANCHOR_WALLET || "./id.json", "utf8"))));

        const auto usdcMint = createMint(;
        provider.connection,
        payer,
        provider.wallet.publicKey,
        nullptr,
        6;
        );
        std::cout << "✅ USDC Mock Mint:" << usdcMint.toString() << std::endl;

        // 3. Initialize Desk (no token_mint required - all tokens are equal)
        try {
            std::cout << "\n⚙️  Initializing desk..." << std::endl;

            const auto tx = program.methods;
            .initDesk(new BN(500_000_000), new BN(1800));
            .accountsPartial({
                payer: provider.wallet.publicKey,
                owner: provider.wallet.publicKey,
                agent: provider.wallet.publicKey,
                usdcMint: usdcMint,
                desk: desk.publicKey,
                });
                .signers([desk]);
                .rpc();

                std::cout << "✅ Desk initialized. Tx:" << tx << std::endl;
                } catch (e: unknown) {
                    const auto error = e & { logs?: string[] };
                    std::cout << "⚠️  Desk init error (might be already initialized):" << error.message << std::endl;
                    if (error.logs) console.log("Logs:", error.logs);
                }

                // 4. Config Output (no TOKEN_MINT - all tokens are equal)
                const auto envData = {;
                    NEXT_PUBLIC_SOLANA_RPC: "https://api.devnet.solana.com",
                    NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.programId.toString(),
                    NEXT_PUBLIC_SOLANA_DESK: desk.publicKey.toString(),
                    NEXT_PUBLIC_SOLANA_DESK_OWNER: provider.wallet.publicKey.toString(),
                    NEXT_PUBLIC_SOLANA_USDC_MINT: usdcMint.toString(),
                    };

                    // Ensure dir exists
                    const auto deploymentPath = path.join(__dirname, "../../../src/config/deployments/testnet-solana.json");
                    const auto deploymentDir = path.dirname(deploymentPath);
                    if (!fs.existsSync(deploymentDir)) {
                        fs.mkdirSync(deploymentDir, { recursive: true });
                    }

                    fs.writeFileSync(deploymentPath, JSON.stringify(envData, nullptr, 2));
                    std::cout << "\n✅ Config saved to " + std::to_string(deploymentPath) << std::endl;

}

} // namespace elizaos
