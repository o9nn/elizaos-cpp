import pkg from "@coral-xyz/anchor";
const anchor: any = pkg as any;
const { BN } = anchor;
import { PublicKey, SystemProgram, Keypair } from "@solana/web3.js";
import {
  getOrCreateAssociatedTokenAccount,
  getAssociatedTokenAddressSync,
} from "@solana/spl-token";
import * as fs from "fs";
import * as path from "path";

// Load .env from parent directory
const envPath = path.join(__dirname, "../../../.env");
if (fs.existsSync(envPath)) {
  const envContent = fs.readFileSync(envPath, "utf8");
  envContent.split("\n").forEach((line) => {
    const match = line.match(/^([^=]+)=(.*)$/);
    if (match) {
      process.env[match[1].trim()] = match[2].trim();
    }
  });
}

async function main() {
  console.log("🚀 Initializing Solana OTC Desk on Mainnet\n");
  console.log("Note: All tokens are equal - no primary token required.");
  console.log("Tokens are registered via TokenRegistry after desk init.\n");

  // Set environment for Anchor
  process.env.ANCHOR_PROVIDER_URL = "https://api.mainnet-beta.solana.com";
  process.env.ANCHOR_WALLET = process.env.ANCHOR_WALLET || 
    path.join(process.env.HOME || "", ".config/solana/mainnet-deployer.json");

  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);

  // Use workspace program
  const program = anchor.workspace.Otc as any;

  // Load owner from wallet
  const walletPath = process.env.ANCHOR_WALLET;
  if (!fs.existsSync(walletPath!)) {
    throw new Error(`Wallet not found at ${walletPath}`);
  }
  const walletData = JSON.parse(fs.readFileSync(walletPath!, "utf8"));
  const owner = Keypair.fromSecretKey(Uint8Array.from(walletData));

  console.log("📋 Program ID:", program.programId.toString());
  console.log("👤 Owner:", owner.publicKey.toString());

  // Check balance
  const balance = await provider.connection.getBalance(owner.publicKey);
  console.log(`💰 Owner balance: ${balance / 1e9} SOL`);

  if (balance < 0.005e9) {
    throw new Error("Insufficient SOL. Need at least 0.005 SOL for initialization.");
  }

  // Use mainnet USDC
  const usdcMint = new PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v");
  console.log("💵 USDC Mint:", usdcMint.toString());

  // Generate agent keypair (can be same as owner for now)
  const agent = owner; // Using owner as agent for simplicity
  console.log("🤖 Agent:", agent.publicKey.toString());

  // Generate desk keypair
  const desk = Keypair.generate();
  console.log("🏦 Desk:", desk.publicKey.toString());

  // Create USDC account for desk (token accounts created per-token via TokenRegistry)
  console.log("\n📦 Creating desk USDC account...");
  const deskUsdcAta = getAssociatedTokenAddressSync(
    usdcMint,
    desk.publicKey,
    true
  );

  await getOrCreateAssociatedTokenAccount(
    provider.connection,
    owner,
    usdcMint,
    desk.publicKey,
    true
  );
  console.log("✅ Desk USDC ATA:", deskUsdcAta.toString());

  // Initialize desk (no token_mint required - all tokens are equal)
  console.log("\n⚙️  Initializing desk...");
  
  const tx = await program.methods
    .initDesk(
      new BN(500_000_000), // $5 minimum
      new BN(1800) // 30 minutes expiry
    )
    .accounts({
      payer: owner.publicKey,
      owner: owner.publicKey,
      agent: agent.publicKey,
      usdcMint: usdcMint,
      desk: desk.publicKey,
      systemProgram: SystemProgram.programId,
    })
    .signers([owner, desk])
    .rpc({ skipPreflight: false });

  console.log("✅ Desk initialized. Tx:", tx);
  console.log(`   View on Solscan: https://solscan.io/tx/${tx}`);

  // Save desk keypair
  const deskKeypairPath = path.join(__dirname, "../desk-mainnet-keypair.json");
  fs.writeFileSync(deskKeypairPath, JSON.stringify(Array.from(desk.secretKey)));
  console.log(`\n✅ Desk keypair saved to ${deskKeypairPath}`);

  // Save config
  const configData = {
    NEXT_PUBLIC_SOLANA_RPC: "https://api.mainnet-beta.solana.com",
    NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.programId.toString(),
    NEXT_PUBLIC_SOLANA_DESK: desk.publicKey.toString(),
    NEXT_PUBLIC_SOLANA_DESK_OWNER: owner.publicKey.toString(),
    NEXT_PUBLIC_SOLANA_USDC_MINT: usdcMint.toString(),
  };

  const deploymentPath = path.join(__dirname, "../../../src/config/deployments/mainnet-solana.json");
  fs.writeFileSync(deploymentPath, JSON.stringify(configData, null, 2));
  console.log(`✅ Config saved to ${deploymentPath}`);

  // Output for .env
  console.log("\n" + "=".repeat(80));
  console.log("🎉 SUCCESS. Update your .env with:");
  console.log("=".repeat(80));
  console.log(`NEXT_PUBLIC_SOLANA_DESK=${desk.publicKey.toString()}`);
  console.log("=".repeat(80));
}

main()
  .then(() => {
    console.log("\n✨ All done.");
    process.exit(0);
  })
  .catch((err) => {
    console.error("\n❌ Error:", err);
    process.exit(1);
  });
