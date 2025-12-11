import type { Program, AnchorProvider as AnchorProviderType } from "@coral-xyz/anchor";
import pkg from "@coral-xyz/anchor";
import { PublicKey, Keypair } from "@solana/web3.js";
import * as fs from "fs";
import type { Otc } from "../target/types/otc";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace } = pkg as typeof import("@coral-xyz/anchor");

async function addApprover() {
  const provider = AnchorProvider.env();
  setProvider(provider);
  const program = workspace.Otc as Program<Otc>;
  
  const ownerData = JSON.parse(fs.readFileSync("./id.json", "utf8"));
  const owner = Keypair.fromSecretKey(Uint8Array.from(ownerData));
  const desk = new PublicKey("7EN1rubej95WmoyupRXQ78PKU2hTCspKn2mVKN1vxuPp");
  
  console.log("Adding owner as approver...");
  console.log("  Owner:", owner.publicKey.toString());
  console.log("  Desk:", desk.toString());
  
  const tx = await program.methods
    .setApprover(owner.publicKey, true)
    .accountsPartial({
      desk,
      owner: owner.publicKey,
    })
    .signers([owner])
    .rpc();
  
  console.log("✅ Owner added as approver, tx:", tx);
}

addApprover()
  .then(() => process.exit(0))
  .catch((err) => {
    console.error("❌ Error:", err);
    process.exit(1);
  });
