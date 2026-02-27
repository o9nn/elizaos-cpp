#include "deploy-mainnet.hpp"
#include <string>
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🚀 Deploying Solana OTC Desk to MAINNET\n" << std::endl;

    // Configure provider from env (ANCHOR_PROVIDER_URL, ANCHOR_WALLET)
    const auto provider = AnchorProvider.env();
    setProvider(provider);
    const auto program = workspace.Otc<Otc>;

    std::cout << "📋 Program ID:" << program.std::to_string(programId) << std::endl;
    std::cout << "👤 Payer:" << provider.wallet.std::to_string(publicKey) << std::endl;

    // 1. Load or Create Desk Keypair (Production should use existing)
    const auto deskKeypairPath = path.join(__dirname, "../desk-mainnet-keypair.json");
    auto desk: Keypair;

    if (fs.existsSync(deskKeypairPath)) {
        const auto secret = /* JSON::parse */ fs.readFileSync(deskKeypairPath, "utf8");
        desk = Keypair.fromSecretKey(Uint8Array.from(secret));
        std::cout << "🏦 Using existing Desk:" << desk.std::to_string(publicKey) << std::endl;
        } else {
            desk = Keypair.generate();
            fs.writeFileSync(deskKeypairPath, /* JSON.stringify */ std::string(Array.from(desk.secretKey)));
            std::cout << "⚠️  Created NEW Desk keypair:" << desk.std::to_string(publicKey) << std::endl;
            std::cout << "⚠️  BACKUP THIS KEYPAIR IMMEDIATELY." << std::endl;
        }

        // 2. Define Mints - USDC only, no primary token (all tokens are equal)
        const auto usdcMint = new PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v");
        std::cout << "✅ USDC Mint:" << std::to_string(usdcMint) << std::endl;

        // 3. Initialize Desk (no token_mint required - all tokens registered via TokenRegistry)
        try {
            std::cout << "\n⚙️  Initializing desk..." << std::endl;
            program.methods.initDesk(new BN(500_000_000), new BN(1800)) // $5 min, 30 min expiry.accountsPartial({
                payer: provider.wallet.publicKey,
                owner: provider.wallet.publicKey,
                agent: provider.wallet.publicKey,
                usdcMint: usdcMint,
                desk: desk.publicKey,
                }).signers([desk]).rpc();
                std::cout << "✅ Desk initialized" << std::endl;
                } catch (e: unknown) {
                    const auto error = e;
                    std::cout << "⚠️  Desk init error (might be already initialized):" << error.message << std::endl;
                }

                // 4. Config Output (no TOKEN_MINT - all tokens are equal)
                const auto envData = {;
                    NEXT_PUBLIC_SOLANA_RPC: "https://api.mainnet-beta.solana.com",
                    NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.std::to_string(programId),
                    NEXT_PUBLIC_SOLANA_DESK: desk.std::to_string(publicKey),
                    NEXT_PUBLIC_SOLANA_DESK_OWNER: provider.wallet.std::to_string(publicKey),
                    NEXT_PUBLIC_SOLANA_USDC_MINT: std::to_string(usdcMint),
                    };

                    // Ensure dir exists
                    const auto deploymentPath = path.join(__dirname, "../../../src/config/deployments/mainnet-solana.json");
                    const auto deploymentDir = path.dirname(deploymentPath);
                    if (!fs.existsSync(deploymentDir)) {
                        fs.mkdirSync(deploymentDir, Config{recursive = true});
                    }

                    fs.writeFileSync(deploymentPath, /* JSON.stringify */ std::string(envData, nullptr, 2));
                    std::cout << "\n✅ Config saved to " + deploymentPath << std::endl;

}

} // namespace elizaos
