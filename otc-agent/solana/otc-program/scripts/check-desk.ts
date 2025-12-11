import type { Program } from "@coral-xyz/anchor";
import pkg from "@coral-xyz/anchor";
import { PublicKey } from "@solana/web3.js";
import type { Otc } from "../target/types/otc";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace } = pkg as typeof import("@coral-xyz/anchor");

async function check() {
  const provider = AnchorProvider.env();
  setProvider(provider);
  const program = workspace.Otc as Program<Otc>;
  
  const desk = new PublicKey("7EN1rubej95WmoyupRXQ78PKU2hTCspKn2mVKN1vxuPp");
  const data = await program.account.desk.fetch(desk);
  
  console.log("Full desk data:", JSON.stringify(data, (_, v) => typeof v === 'bigint' ? v.toString() : v, 2));
}

check().catch(console.error);
