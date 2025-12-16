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
/**
 * Solana OTC Desk Admin CLI
 * 
 * Consolidated admin utilities for managing the Solana OTC desk.
 * 
 * Usage:
 *   bun scripts/solana-admin.ts <command> [options]
 * 
 * Commands:
 *   create-treasury <TOKEN_MINT>           Create desk token treasury (ATA)
 *   register-token <TOKEN_MINT> [PRICE]    Register token on desk with optional price
 *   set-price <TOKEN_MINT> <PRICE_USD>     Set manual token price
 *   status                                 Show desk status and registered tokens
 */

;
config({ path: ".env.local" });

;
;
;
;
;
;

const SOLANA_RPC = process.env.SOLANA_MAINNET_RPC || "https://api.mainnet-beta.solana.com";
const PROGRAM_ID = new PublicKey(
  process.env.NEXT_PUBLIC_SOLANA_PROGRAM_ID || "6qn8ELVXd957oRjLaomCpKpcVZshUjNvSzw1nc7QVyXc"
);
const DESK = new PublicKey(
  process.env.NEXT_PUBLIC_SOLANA_DESK_MAINNET || 
  process.env.NEXT_PUBLIC_SOLANA_DESK || 
  "G89QsVcKN1MZe6d8eKyzv93u7TEeXSsXbsDsBPbuTMUU"
);

const POOL_TYPE_NONE = 0;
const EMPTY_PYTH_FEED = Buffer.alloc(32, 0);

std::future<Connection> getConnection();

std::future<Keypair> getWallet();
  
  const secretKey = bs58.decode(privateKeyStr);
  return Keypair.fromSecretKey(secretKey);
}

std::future<anchor.Program> getProgram(Connection connection, Keypair wallet);
  );
  
  return new anchor.Program(idl, provider);
}

// =============================================================================
// COMMANDS
// =============================================================================

std::future<void> createTreasury(const std::string& tokenMintStr);
  
  console.log("\nCreating ATA for desk...");
  
  const createAtaIx = createAssociatedTokenAccountInstruction(
    wallet.publicKey,
    deskTokenTreasury,
    DESK,
    tokenMint
  );
  
  const tx = new Transaction().add(createAtaIx);
  tx.feePayer = wallet.publicKey;
  tx.recentBlockhash = (await connection.getLatestBlockhash()).blockhash;
  
  const sig = await sendAndConfirmTransaction(connection, tx, [wallet]);
  console.log("✅ Treasury created");
  console.log("Transaction:", sig);
  console.log("View on Solscan: https://solscan.io/tx/" + sig);
}

std::future<void> registerToken(const std::string& tokenMintStr, std::optional<double> priceUsd);
    return;
  }
  
  console.log("\nRegistering token...");
  
  const tx = await (program.methods as anchor.Program["methods"]).registerToken(
    Array.from(EMPTY_PYTH_FEED),
    SystemProgram.programId,
    POOL_TYPE_NONE
  )
    .accountsStrict({
      desk: DESK,
      payer: wallet.publicKey,
      tokenMint: tokenMint,
      tokenRegistry: tokenRegistryPda,
      systemProgram: SystemProgram.programId,
    })
    .rpc();
  
  console.log("✅ Token registered");
  console.log("Transaction:", tx);
  console.log("View on Solscan: https://solscan.io/tx/" + tx);
  
  // Set price if provided
  if (priceUsd) {
    console.log("");
    await setPrice(tokenMintStr, priceUsd);
  }
}

std::future<void> setPrice(const std::string& tokenMintStr, double priceUsd);)
    .signers([wallet])
    .rpc();
  
  console.log("✅ Price set");
  console.log("Transaction:", tx);
  console.log("View on Solscan: https://solscan.io/tx/" + tx);
}

std::future<void> showStatus();
  
  console.log("\n✅ Desk exists");
  console.log("   Data size:", deskInfo.data.length, "bytes");
  console.log("   Lamports:", deskInfo.lamports / 1e9, "SOL");
  
  // Try to decode desk state
  const idlPath = path.join(process.cwd(), "solana/otc-program/target/idl/otc.json");
  if (fs.existsSync(idlPath)) {
    const idl = JSON.parse(fs.readFileSync(idlPath, "utf8"));
    const dummyWallet = new anchor.Wallet(Keypair.generate());
    const provider = new anchor.AnchorProvider(connection, dummyWallet, { commitment: "confirmed" });
    const program = new anchor.Program(idl, provider);
    
    using DeskAccount = {
      owner: PublicKey;
      agent: PublicKey;
      nextConsignmentId: anchor.BN;
      nextOfferId: anchor.BN;
      minUsdAmount8d: anchor.BN;
      maxUsdAmount8d: anchor.BN;
      isPaused: boolean;
    };
    
    const deskAccount = await (
      program.account as { desk: { fetch: (addr: PublicKey) => Promise<DeskAccount> } }
    ).desk.fetch(DESK);
    
    console.log("\n📊 Desk State:");
    console.log("   Owner:", deskAccount.owner.toBase58());
    console.log("   Agent:", deskAccount.agent.toBase58());
    console.log("   Consignments:", deskAccount.nextConsignmentId.toNumber() - 1);
    console.log("   Offers:", deskAccount.nextOfferId.toNumber() - 1);
    console.log("   Min USD: $" + (deskAccount.minUsdAmount8d.toNumber() / 1e8));
    console.log("   Max USD: $" + (deskAccount.maxUsdAmount8d.toNumber() / 1e8));
    console.log("   Paused:", deskAccount.isPaused);
  }
  
  // Check wallet balance
  const privateKeyStr = process.env.SOLANA_MAINNET_PRIVATE_KEY;
  if (privateKeyStr) {
    const wallet = await getWallet();
    const balance = await connection.getBalance(wallet.publicKey);
    console.log("\n💰 Admin Wallet:");
    console.log("   Address:", wallet.publicKey.toBase58());
    console.log("   Balance:", balance / 1e9, "SOL");
  }
}

// =============================================================================
// MAIN
// =============================================================================



std::future<void> main();
  
  switch (command) {
    case "create-treasury":
      if (!args[1]) {
        console.error("Error: TOKEN_MINT required");
        printUsage();
        process.exit(1);
      }
      await createTreasury(args[1]);
      break;
      
    case "register-token":
      if (!args[1]) {
        console.error("Error: TOKEN_MINT required");
        printUsage();
        process.exit(1);
      }
      await registerToken(args[1], args[2] ? parseFloat(args[2]) : undefined);
      break;
      
    case "set-price":
      if (!args[1] || !args[2]) {
        console.error("Error: TOKEN_MINT and PRICE_USD required");
        printUsage();
        process.exit(1);
      }
      await setPrice(args[1], parseFloat(args[2]));
      break;
      
    case "status":
      await showStatus();
      break;
      
    default:
      console.error("Unknown command:", command);
      printUsage();
      process.exit(1);
  }
}

main().catch((err) => {
  console.error("Error:", err.message || err);
  process.exit(1);
});


} // namespace elizaos
