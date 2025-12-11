import type { Program } from "@coral-xyz/anchor";
import pkg from "@coral-xyz/anchor";
import { PublicKey, SystemProgram, Keypair } from "@solana/web3.js";
import {
  TOKEN_PROGRAM_ID,
  createMint,
  getOrCreateAssociatedTokenAccount,
  mintTo,
  getAssociatedTokenAddressSync,
} from "@solana/spl-token";
import * as fs from "fs";
import { fileURLToPath } from "url";
import * as path from "path";
import type { Otc } from "../target/types/otc";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace, BN } = pkg as typeof import("@coral-xyz/anchor");

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

async function main() {
  console.log("🚀 Quick Solana OTC Desk Setup (LOCAL TESTING ONLY)\n");
  console.log("⚠️  This script uses setManualTokenPrice which requires the 'testing' feature.");
  console.log("⚠️  Build with: anchor build -- --features testing");
  console.log("⚠️  Production uses Pyth oracle or on-chain pool pricing instead.\n");
  console.log("Note: All tokens are equal - registered via TokenRegistry.\n");

  const provider = AnchorProvider.env();
  setProvider(provider);
  const program = workspace.Otc as Program<Otc>;

  console.log("📋 Program ID:", program.programId.toString());

  // Load owner keypair
  const ownerData = JSON.parse(fs.readFileSync("./id.json", "utf8"));
  const owner = Keypair.fromSecretKey(Uint8Array.from(ownerData));
  console.log("👤 Owner:", owner.publicKey.toString());

  // Check balance
  let balance = await provider.connection.getBalance(owner.publicKey);
  console.log(`💰 Owner balance: ${balance / 1e9} SOL`);

  if (balance < 3e9) {
    console.log("💸 Airdropping SOL...");
    const sig = await provider.connection.requestAirdrop(owner.publicKey, 5e9);
    await provider.connection.confirmTransaction(sig, "confirmed");
    balance = await provider.connection.getBalance(owner.publicKey);
    console.log(`💰 New balance: ${balance / 1e9} SOL`);
  }

  // Generate agent and desk keypairs
  const agent = Keypair.generate();
  const agentSig = await provider.connection.requestAirdrop(
    agent.publicKey,
    1e9
  );
  await provider.connection.confirmTransaction(agentSig, "confirmed");
  console.log("🤖 Agent:", agent.publicKey.toString());

  const desk = Keypair.generate();
  console.log("🏦 Desk (keypair):", desk.publicKey.toString());

  // Create token mints for testing
  console.log("\n🪙 Creating test token mints...");
  console.log("⚠️  NOTE: Using 9 decimals for native Solana token (standard)");
  
  const tokenMint = await createMint(
    provider.connection,
    owner,
    owner.publicKey,
    null,
    9  // Standard Solana token decimals
  );
  console.log("✅ Token Mint:", tokenMint.toString());

  const usdcMint = await createMint(
    provider.connection,
    owner,
    owner.publicKey,
    null,
    6
  );
  console.log("✅ USDC Mint:", usdcMint.toString());

  // Create token accounts for desk
  console.log("\n📦 Creating desk token accounts...");
  const deskTokenAta = getAssociatedTokenAddressSync(
    tokenMint,
    desk.publicKey,
    true
  );
  const deskUsdcAta = getAssociatedTokenAddressSync(
    usdcMint,
    desk.publicKey,
    true
  );

  await getOrCreateAssociatedTokenAccount(
    provider.connection,
    owner,
    tokenMint,
    desk.publicKey,
    true
  );
  console.log("✅ Desk token ATA:", deskTokenAta.toString());

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
      new BN(500_000_000),
      new BN(1800)
    )
    .accountsPartial({
      payer: owner.publicKey,
      owner: owner.publicKey,
      agent: agent.publicKey,
      usdcMint: usdcMint,
      desk: desk.publicKey,
    })
    .signers([owner, desk])
    .rpc();

  console.log("✅ Desk initialized. Tx:", tx);

  // Register test token via TokenRegistry
  console.log("\n📝 Registering test token via TokenRegistry...");
  const [tokenRegistryPda] = PublicKey.findProgramAddressSync(
    [Buffer.from("registry"), desk.publicKey.toBuffer(), tokenMint.toBuffer()],
    program.programId
  );

  await program.methods
    .registerToken(
      Array(32).fill(0), // No Pyth feed for test token
      PublicKey.default, // No pool for test token
      0                  // PoolType::None (0=None, 1=Raydium, 2=Orca, 3=PumpSwap)
    )
    .accountsPartial({
      desk: desk.publicKey,
      payer: owner.publicKey,
      tokenMint: tokenMint,
      tokenRegistry: tokenRegistryPda,
    })
    .signers([owner])
    .rpc();
  console.log("✅ Token registered in TokenRegistry:", tokenRegistryPda.toString());

  // Set manual price on token registry (for testing)
  console.log("\n💲 Setting manual price on token registry...");
  await program.methods
    .setManualTokenPrice(new BN(1_000_000_000)) // $10 per token (8 decimals)
    .accountsPartial({
      tokenRegistry: tokenRegistryPda,
      desk: desk.publicKey,
      owner: owner.publicKey,
    })
    .signers([owner])
    .rpc();
  console.log("✅ Token price set");

  // Set SOL price on desk
  console.log("\n💲 Setting SOL price...");
  await program.methods
    .setPrices(
      new BN(1_000_000_000),      // token price (deprecated, kept for compatibility)
      new BN(100_000_000_00),     // SOL price: $100 (8 decimals)
      new BN(0),                   // updated_at (ignored, uses clock)
      new BN(3600)                 // max age
    )
    .accountsPartial({ desk: desk.publicKey, owner: owner.publicKey })
    .signers([owner])
    .rpc();
  console.log("✅ SOL price set");

  // Add owner as approver
  console.log("\n👤 Adding owner as approver...");
  await program.methods
    .setApprover(owner.publicKey, true)
    .accountsPartial({ desk: desk.publicKey, owner: owner.publicKey })
    .signers([owner])
    .rpc();
  console.log("✅ Owner added as approver");

  // Mint tokens to owner
  console.log("\n💎 Minting tokens...");
  const ownerTokenAta = await getOrCreateAssociatedTokenAccount(
    provider.connection,
    owner,
    tokenMint,
    owner.publicKey
  );

  await mintTo(
    provider.connection,
    owner,
    tokenMint,
    ownerTokenAta.address,
    owner,
    1_000_000_000_000_000
  );
  console.log("✅ Minted 1,000,000 tokens to owner");

  // Deposit to desk (now requires token_registry)
  console.log("\n📥 Depositing tokens to desk...");
  await program.methods
    .depositTokens(new BN("500000000000000"))
    .accountsPartial({
      desk: desk.publicKey,
      tokenRegistry: tokenRegistryPda,
      owner: owner.publicKey,
      ownerTokenAta: ownerTokenAta.address,
      deskTokenTreasury: deskTokenAta,
    })
    .signers([owner])
    .rpc();
  console.log("✅ Deposited 500,000 tokens");

  // Save desk keypair
  const deskKeypairPath = path.join(__dirname, "../desk-keypair.json");
  fs.writeFileSync(deskKeypairPath, JSON.stringify(Array.from(desk.secretKey)));
  console.log("\n💾 Saved desk keypair to:", deskKeypairPath);

  // Output for .env (no TOKEN_MINT - all tokens are equal)
  console.log("\n" + "=".repeat(80));
  console.log("🎉 SUCCESS. Update your .env.local with these values:");
  console.log("=".repeat(80));
  console.log(`NEXT_PUBLIC_SOLANA_RPC=http://127.0.0.1:8899`);
  console.log(`NEXT_PUBLIC_SOLANA_PROGRAM_ID=${program.programId.toString()}`);
  console.log(`NEXT_PUBLIC_SOLANA_DESK=${desk.publicKey.toString()}`);
  console.log(`NEXT_PUBLIC_SOLANA_DESK_OWNER=${owner.publicKey.toString()}`);
  console.log(`NEXT_PUBLIC_SOLANA_USDC_MINT=${usdcMint.toString()}`);
  console.log("=".repeat(80));
  console.log("\nNote: Test token mint for local testing:", tokenMint.toString());
  console.log("=".repeat(80));

  // Write to src config (no TOKEN_MINT)
  const deploymentPath = path.join(__dirname, "../../../src/config/deployments/local-solana.json");
  const deploymentDir = path.dirname(deploymentPath);
  if (!fs.existsSync(deploymentDir)) {
    fs.mkdirSync(deploymentDir, { recursive: true });
  }

  const envData = {
    NEXT_PUBLIC_SOLANA_RPC: "http://127.0.0.1:8899",
    NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.programId.toString(),
    NEXT_PUBLIC_SOLANA_DESK: desk.publicKey.toString(),
    NEXT_PUBLIC_SOLANA_DESK_OWNER: owner.publicKey.toString(),
    NEXT_PUBLIC_SOLANA_USDC_MINT: usdcMint.toString(),
  };
  
  fs.writeFileSync(
    deploymentPath,
    JSON.stringify(envData, null, 2)
  );
  console.log(`\n✅ Config saved to ${deploymentPath}`);

  // Update .env.local
  const envLocalPath = path.join(__dirname, "../../../.env.local");
  let envContent = "";
  if (fs.existsSync(envLocalPath)) {
    envContent = fs.readFileSync(envLocalPath, "utf8");
  }

  // Update or add each env var (no TOKEN_MINT)
  const envVars = {
    NEXT_PUBLIC_SOLANA_RPC: "http://127.0.0.1:8899",
    NEXT_PUBLIC_SOLANA_PROGRAM_ID: program.programId.toString(),
    NEXT_PUBLIC_SOLANA_DESK: desk.publicKey.toString(),
    NEXT_PUBLIC_SOLANA_DESK_OWNER: owner.publicKey.toString(),
    NEXT_PUBLIC_SOLANA_USDC_MINT: usdcMint.toString(),
  };

  for (const [key, value] of Object.entries(envVars)) {
    const regex = new RegExp(`^${key}=.*$`, "m");
    if (regex.test(envContent)) {
      envContent = envContent.replace(regex, `${key}=${value}`);
    } else {
      envContent += `\n${key}=${value}`;
    }
  }

  // Remove old TOKEN_MINT if present
  envContent = envContent.replace(/^NEXT_PUBLIC_SOLANA_TOKEN_MINT=.*$/m, "");
  envContent = envContent.replace(/\n\n+/g, "\n\n"); // Clean up extra newlines

  fs.writeFileSync(envLocalPath, envContent.trim() + "\n");
  console.log(`✅ Updated ${envLocalPath}`);
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
