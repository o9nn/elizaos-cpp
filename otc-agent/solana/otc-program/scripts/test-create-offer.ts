import type { Program } from "@coral-xyz/anchor";
import pkg from "@coral-xyz/anchor";
import { PublicKey, Keypair, SystemProgram } from "@solana/web3.js";
import { getAssociatedTokenAddressSync } from "@solana/spl-token";
import * as fs from "fs";
import type { Otc } from "../target/types/otc";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace, BN } = pkg as typeof import("@coral-xyz/anchor");

async function test() {
  const provider = AnchorProvider.env();
  setProvider(provider);
  const program = workspace.Otc as Program<Otc>;

  const ownerData = JSON.parse(fs.readFileSync("./id.json", "utf8"));
  const owner = Keypair.fromSecretKey(Uint8Array.from(ownerData));

  const desk = new PublicKey("7EN1rubej95WmoyupRXQ78PKU2hTCspKn2mVKN1vxuPp");
  const tokenMint = new PublicKey(
    "5QLcgYPdVSgZrS84TecffohCJHWC1eEkftnK6DKtpxg9"
  );
  const deskTokenTreasury = getAssociatedTokenAddressSync(
    tokenMint,
    desk,
    true
  );

  const offer = Keypair.generate();

  console.log("Testing createOffer with 1000 tokens...");

  const tx = await program.methods
    .createOffer(
      new BN("1000000000000"), // 1000 tokens * 10^9
      1000, // 10% discount
      0, // SOL payment
      new BN(15552000) // 180 days
    )
    .accountsPartial({
      desk,
      deskTokenTreasury,
      beneficiary: owner.publicKey,
      offer: offer.publicKey,
    })
    .signers([offer])
    .rpc();

  console.log("✅ Offer created successfully:", tx);
}

test().catch(console.error);
