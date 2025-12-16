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

import type { Program } from "@coral-xyz/anchor";
;
;
;
;
;
import type { Otc } from "../target/types/otc";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace, BN } = pkg as typeof import("@coral-xyz/anchor");

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

async  else {
    desk = Keypair.generate();
    fs.writeFileSync(deskKeypairPath, JSON.stringify(Array.from(desk.secretKey)));
    console.log("⚠️  Created NEW Desk keypair:", desk.publicKey.toString());
    console.log("⚠️  BACKUP THIS KEYPAIR IMMEDIATELY.");
  }

  // 2. Define Mints - USDC only, no primary token (all tokens are equal)
  const usdcMint = new PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v");
  console.log("✅ USDC Mint:", usdcMint.toString());

  // 3. Initialize Desk (no token_mint required - all tokens registered via TokenRegistry)
  try {
    console.log("\n⚙️  Initializing desk...");
    await program.methods
      .initDesk(new BN(500_000_000), new BN(1800)) // $5 min, 30 min expiry
      .accountsPartial({
        payer: provider.wallet.publicKey,
        owner: provider.wallet.publicKey,
        agent: provider.wallet.publicKey, 
        usdcMint: usdcMint,
        desk: desk.publicKey,
      })
      .signers([desk])
      .rpc();
    console.log("✅ Desk initialized");
  } catch (e: unknown) {
    const error = e as Error;
    console.log("⚠️  Desk init error (might be already initialized):", error.message);
  }

  // 4. Config Output (no TOKEN_MINT - all tokens are equal)
  const envData = {
    NEXT_PUBLIC_SOLANA_RPC: "https://api.mainnet-beta.solana.com",
    NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.programId.toString(),
    NEXT_PUBLIC_SOLANA_DESK: desk.publicKey.toString(),
    NEXT_PUBLIC_SOLANA_DESK_OWNER: provider.wallet.publicKey.toString(),
    NEXT_PUBLIC_SOLANA_USDC_MINT: usdcMint.toString(),
  };

  // Ensure dir exists
  const deploymentPath = path.join(__dirname, "../../../src/config/deployments/mainnet-solana.json");
  const deploymentDir = path.dirname(deploymentPath);
  if (!fs.existsSync(deploymentDir)) {
    fs.mkdirSync(deploymentDir, { recursive: true });
  }

  fs.writeFileSync(deploymentPath, JSON.stringify(envData, null, 2));
  console.log(`\n✅ Config saved to ${deploymentPath}`);
}

main().then(() => process.exit(0)).catch(e => { console.error(e); process.exit(1); });

} // namespace elizaos
