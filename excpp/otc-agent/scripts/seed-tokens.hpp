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

#!/usr/bin/env bun
;

/**
 * Token Seeding Script - LOCAL DEVELOPMENT ONLY
 * 
 * This script seeds test tokens for local Anvil development.
 * It will NEVER seed to production databases.
 * 
 * Safety checks:
 * 1. Must be explicitly on "localnet" network
 * 2. Must be targeting localhost API
 * 3. Skips if any production indicators are detected
 */

async );
  }

  // === SAFETY CHECK 1: Skip if SKIP_SEED is set ===
  if (process.env.SKIP_SEED === "true") {
    console.log("\n⏭️  SKIP_SEED=true - skipping token seeding\n");
    process.exit(0);
  }
  
  // === SAFETY CHECK 2: Only allow localnet ===
  const network = process.env.NETWORK || process.env.NEXT_PUBLIC_NETWORK || "localnet";
  if (network !== "localnet") {
    console.log(`\n🛑 BLOCKED: Network is "${network}", not "localnet"`);
    console.log("   Token seeding is ONLY allowed for local development.");
    console.log("   Production tokens must be registered by their actual owners via the UI.\n");
    process.exit(0);
  }
  
  // === SAFETY CHECK 3: Verify localhost API ===
  const apiUrl = process.env.NEXT_PUBLIC_URL || "http://localhost:4444";
  const isLocalhost = apiUrl.includes("localhost") || apiUrl.includes("127.0.0.1");
  
  if (!isLocalhost) {
    console.log(`\n🛑 BLOCKED: API URL is "${apiUrl}", not localhost`);
    console.log("   Token seeding is ONLY allowed when targeting localhost.");
    console.log("   This prevents accidental seeding to production databases.\n");
    process.exit(0);
  }

  // === SAFETY CHECK 4: Block if DATABASE_URL looks like production ===
  const dbUrl = process.env.DATABASE_URL || "";
  const looksLikeProduction = 
    dbUrl.includes("neon.tech") || 
    dbUrl.includes("supabase") || 
    dbUrl.includes("planetscale") ||
    dbUrl.includes("railway") ||
    dbUrl.includes("render.com") ||
    dbUrl.includes("aws") ||
    dbUrl.includes("azure") ||
    dbUrl.includes("gcp");
    
  if (looksLikeProduction) {
    console.log("\n🛑 BLOCKED: DATABASE_URL appears to be a production database");
    console.log("   Token seeding is ONLY allowed for local development.\n");
    process.exit(0);
  }

  console.log("\n🌱 Seeding LOCAL development marketplace...\n");
  console.log("   Network: localnet");
  console.log("   API: localhost");
  console.log("");
  
  // Check for local EVM deployment
  const evmDeploymentPath = "./contracts/deployments/eliza-otc-deployment.json";
  
  if (!fs.existsSync(evmDeploymentPath)) {
    console.log("⚠️  Local contracts not deployed yet, skipping seed");
    console.log("   Run 'npm run dev' to deploy contracts first\n");
    process.exit(0);
  }

  // Wait for local frontend
  let retries = 5;
  while (retries > 0) {
    const healthCheck = await fetch("http://localhost:4444/api/tokens").catch(() => null);
    if (healthCheck && healthCheck.ok) {
      console.log("✅ Local frontend is ready");
      break;
    }
    console.log(`⏳ Waiting for local frontend... (${retries} retries left)`);
    await new Promise(r => setTimeout(r, 2000));
    retries--;
  }

  if (retries === 0) {
    console.log("⚠️  Local frontend not ready, skipping seed\n");
    process.exit(0);
  }

  // --- EVM Local Seeding (Anvil only) ---
  const localEvmPath = "./src/config/deployments/local-evm.json";
  if (fs.existsSync(localEvmPath)) {
    const evmDeployment = JSON.parse(fs.readFileSync(localEvmPath, "utf8"));
    
    if (!evmDeployment.contracts?.elizaToken) {
      console.log("⚠️  No local EVM token deployed, skipping EVM seed");
    } else {
      const testTokenAddress = evmDeployment.contracts.elizaToken;
      const anvilAccount = "0xf39Fd6e51aad88F6F4ce6aB8827279cffFb92266";

      console.log(`\n[Local EVM] Test token: ${testTokenAddress}`);
      
      // Register local test token
      await fetch("http://localhost:4444/api/tokens", {
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

      console.log("✅ [Local EVM] Test token registered");

      // Create test consignment
      const tokenId = `token-base-${testTokenAddress.toLowerCase()}`;
      
      await fetch("http://localhost:4444/api/consignments", {
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

      console.log("✅ [Local EVM] Test consignment created");
    }
  } else {
    console.log("⚠️  No local EVM deployment found, skipping EVM seed");
  }

  // Solana: Never seed fake tokens
  console.log("\n[Solana] No seeding - tokens registered by owners via TokenRegistry");

  console.log("\n✅ Local development seeding complete");
  console.log("   Visit http://localhost:4444 to test\n");
}

seedTokens().catch((err) => {
  console.error("❌ Seed error:", err);
  process.exit(1);
});

} // namespace elizaos
