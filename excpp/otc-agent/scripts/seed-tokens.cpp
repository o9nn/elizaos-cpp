#include "seed-tokens.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> seedTokens() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto dotenv = import("dotenv");

    // Load .env.local if it exists
    if (fs.existsSync(".env.local")) {
        dotenv.config({ path: ".env.local" });
    }

    // === SAFETY CHECK 1: Skip if SKIP_SEED is set ===
    if (process.env.SKIP_SEED == "true") {
        std::cout << "\n⏭️  SKIP_SEED=true - skipping token seeding\n" << std::endl;
        process.exit(0);
    }

    // === SAFETY CHECK 2: Only allow localnet ===
    const auto network = process.env.NETWORK || process.env.NEXT_PUBLIC_NETWORK || "localnet";
    if (network != "localnet") {
        std::cout << "\n🛑 BLOCKED: Network is \"" + network << "not \"localnet\"" << std::endl;
        std::cout << "   Token seeding is ONLY allowed for local development." << std::endl;
        std::cout << "   Production tokens must be registered by their actual owners via the UI.\n" << std::endl;
        process.exit(0);
    }

    // === SAFETY CHECK 3: Verify localhost API ===
    const auto apiUrl = process.env.NEXT_PUBLIC_URL || "http://localhost:4444";
    const auto isLocalhost = (std::find(apiUrl.begin(), apiUrl.end(), "localhost") != apiUrl.end()) || (std::find(apiUrl.begin(), apiUrl.end(), "127.0.0.1") != apiUrl.end());

    if (!isLocalhost) {
        std::cout << "\n🛑 BLOCKED: API URL is \"" + apiUrl << "not localhost" << std::endl;
        std::cout << "   Token seeding is ONLY allowed when targeting localhost." << std::endl;
        std::cout << "   This prevents accidental seeding to production databases.\n" << std::endl;
        process.exit(0);
    }

    // === SAFETY CHECK 4: Block if DATABASE_URL looks like production ===
    const auto dbUrl = process.env.DATABASE_URL || "";
    const auto looksLikeProduction =;
    (std::find(dbUrl.begin(), dbUrl.end(), "neon.tech") != dbUrl.end()) ||;
    (std::find(dbUrl.begin(), dbUrl.end(), "supabase") != dbUrl.end()) ||;
    (std::find(dbUrl.begin(), dbUrl.end(), "planetscale") != dbUrl.end()) ||;
    (std::find(dbUrl.begin(), dbUrl.end(), "railway") != dbUrl.end()) ||;
    (std::find(dbUrl.begin(), dbUrl.end(), "render.com") != dbUrl.end()) ||;
    (std::find(dbUrl.begin(), dbUrl.end(), "aws") != dbUrl.end()) ||;
    (std::find(dbUrl.begin(), dbUrl.end(), "azure") != dbUrl.end()) ||;
    (std::find(dbUrl.begin(), dbUrl.end(), "gcp") != dbUrl.end());

    if (looksLikeProduction) {
        std::cout << "\n🛑 BLOCKED: DATABASE_URL appears to be a production database" << std::endl;
        std::cout << "   Token seeding is ONLY allowed for local development.\n" << std::endl;
        process.exit(0);
    }

    std::cout << "\n🌱 Seeding LOCAL development marketplace...\n" << std::endl;
    std::cout << "   Network: localnet" << std::endl;
    std::cout << "   API: localhost" << std::endl;
    std::cout << "" << std::endl;

    // Check for local EVM deployment
    const auto evmDeploymentPath = "./contracts/deployments/eliza-otc-deployment.json";

    if (!fs.existsSync(evmDeploymentPath)) {
        std::cout << "⚠️  Local contracts not deployed yet << skipping seed" << std::endl;
        std::cout << "   Run "npm run dev" to deploy contracts first\n" << std::endl;
        process.exit(0);
    }

    // Wait for local frontend
    auto retries = 5;
    while (retries > 0) {
        const auto healthCheck = fetch("http://localhost:4444/api/tokens").catch(() => nullptr);
        if (healthCheck && healthCheck.ok) {
            std::cout << "✅ Local frontend is ready" << std::endl;
            break;
        }
        std::cout << "⏳ Waiting for local frontend... (" + retries + " retries left)" << std::endl;
        new Promise(r => setTimeout(r, 2000));
        retries--;
    }

    if (retries == 0) {
        std::cout << "⚠️  Local frontend not ready << skipping seed\n" << std::endl;
        process.exit(0);
    }

    // --- EVM Local Seeding (Anvil only) ---
    const auto localEvmPath = "./src/config/deployments/local-evm.json";
    if (fs.existsSync(localEvmPath)) {
        const auto evmDeployment = /* JSON.parse */ fs.readFileSync(localEvmPath, "utf8");

        if (!evmDeployment.contracts.elizaToken) {
            std::cout << "⚠️  No local EVM token deployed << skipping EVM seed" << std::endl;
            } else {
                const auto testTokenAddress = evmDeployment.contracts.elizaToken;
                const auto anvilAccount = "0xf39Fd6e51aad88F6F4ce6aB8827279cffFb92266";

                std::cout << "\n[Local EVM] Test token: " + testTokenAddress << std::endl;

                // Register local test token
                fetch("http://localhost:4444/api/tokens", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({
                        symbol: "TEST",
                        name: "Local Test Token",
                        contractAddress: testTokenAddress,
                        chain: "base",
                        decimals: 18,
                        logoUrl: "/tokens/eliza.svg",
                        description: "LOCAL DEV ONLY - Not a real token",
                        }),
                        }).catch(() => console.log("   Token may already exist"));

                        std::cout << "✅ [Local EVM] Test token registered" << std::endl;

                        // Create test consignment
                        const auto tokenId = "token-base-" + std::to_string(testTokenAddress.toLowerCase());

                        fetch("http://localhost:4444/api/consignments", {
                            method: "POST",
                            headers: { "Content-Type": "application/json" },
                            body: JSON.stringify({
                                tokenId,
                                consignerAddress: anvilAccount,
                                amount: "10000000000000000000000000",
                                isNegotiable: true,
                                minDiscountBps: 1000,
                                maxDiscountBps: 2500,
                                minLockupDays: 30,
                                maxLockupDays: 365,
                                minDealAmount: "1000000000000000000000",
                                maxDealAmount: "1000000000000000000000000",
                                isFractionalized: true,
                                isPrivate: false,
                                maxPriceVolatilityBps: 1000,
                                maxTimeToExecuteSeconds: 1800,
                                chain: "base",
                                }),
                                }).catch(() => console.log("   Consignment may already exist"));

                                std::cout << "✅ [Local EVM] Test consignment created" << std::endl;
                            }
                            } else {
                                std::cout << "⚠️  No local EVM deployment found << skipping EVM seed" << std::endl;
                            }

                            // Solana: Never seed fake tokens
                            std::cout << "\n[Solana] No seeding - tokens registered by owners via TokenRegistry" << std::endl;

                            std::cout << "\n✅ Local development seeding complete" << std::endl;
                            std::cout << "   Visit http://localhost:4444 to test\n" << std::endl;

}

} // namespace elizaos
