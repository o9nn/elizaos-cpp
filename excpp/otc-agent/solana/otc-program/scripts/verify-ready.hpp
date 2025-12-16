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
import type { Otc } from "../target/types/otc";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace } = pkg as typeof import("@coral-xyz/anchor");

async `);
  console.log(`   SOL: $${solPrice.toFixed(2)}`);
  console.log(`   Age: ${pricesAge}s (max: ${maxAge}s)`);
  console.log(`   Status: ${pricesAge < maxAge ? "FRESH ✅" : "STALE ❌"}\n`);
  
  console.log("✅ Inventory:");
  console.log(`   Available: ${available} tokens`);
  console.log(`   Reserved: ${reserved} tokens`);
  console.log(`   Status: ${available > 0 ? "READY ✅" : "EMPTY ❌"}\n`);
  
  console.log("✅ Approvers:");
  console.log(`   Count: ${data.approvers.length}`);
  console.log(`   List: ${data.approvers.map((a: PublicKey) => a.toString().slice(0, 8)).join(", ")}\n`);
  
  console.log("✅ Configuration:");
  console.log(`   Paused: ${data.paused ? "YES ❌" : "NO ✅"}`);
  console.log(`   Next Offer ID: ${parseInt(data.nextOfferId.toString(), 16)}\n`);
  
  if (pricesAge < maxAge && available > 0 && !data.paused && data.approvers.length > 0) {
    console.log("🎉 LOCALNET READY FOR TRADING");
  } else {
    console.log("⚠️  LOCALNET NOT READY - check items above");
    process.exit(1);
  }
}

verify().catch((err) => {
  console.error("❌ Verification failed:", err.message);
  process.exit(1);
});

} // namespace elizaos
