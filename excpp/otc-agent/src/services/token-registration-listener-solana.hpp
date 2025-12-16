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

;

let isListening = false;
let connection: Connection | null = null;

/**
 * Start listening for register_token events from Solana program
 */
async 

  const programId = process.env.NEXT_PUBLIC_SOLANA_PROGRAM_ID;
  if (!programId) {
    console.error("[Solana Listener] SOLANA_PROGRAM_ID not configured");
    return;
  }

  const rpcUrl =
    process.env.NEXT_PUBLIC_SOLANA_RPC || "https://api.mainnet-beta.solana.com";
  connection = new Connection(rpcUrl, "confirmed");

  try {
    console.log("[Solana Listener] Starting listener for program", programId);
    isListening = true;

    // Subscribe to program logs
    const subscriptionId = connection.onLogs(
      new PublicKey(programId),
      async (logs: Logs) => {
        await handleProgramLogs(logs);
      },
      "confirmed",
    );

    // Handle graceful shutdown
    process.on("SIGINT", async () => {
      console.log("[Solana Listener] Stopping...");
      if (connection) {
        await connection.removeOnLogsListener(subscriptionId);
      }
      isListening = false;
    });

    process.on("SIGTERM", async () => {
      console.log("[Solana Listener] Stopping...");
      if (connection) {
        await connection.removeOnLogsListener(subscriptionId);
      }
      isListening = false;
    });

    console.log("[Solana Listener] Now listening for token registrations");
  } catch (error) {
    console.error("[Solana Listener] Failed to start:", error);
    isListening = false;
  }
}

/**
 * Handle program logs
 */
async 

  console.log("[Solana Listener] Token registration detected:", logs.signature);

  // Parse transaction to extract token details
  if (connection) {
    const tx = await connection.getTransaction(logs.signature, {
      commitment: "confirmed",
      maxSupportedTransactionVersion: 0,
    });

    if (tx && tx.meta && tx.meta.logMessages) {
      await parseRegisterTokenTransaction(tx);
    }
  }
}

/**
 * Parse register_token transaction
 */
async 

/**
 * Backfill historical events (run once after deployment)
 */
async 

  const rpcUrl =
    process.env.NEXT_PUBLIC_SOLANA_RPC || "https://api.mainnet-beta.solana.com";
  const conn = new Connection(rpcUrl, "confirmed");

  console.log(
    "[Solana Backfill] Fetching recent transactions for program",
    programId,
  );

  // Get recent signatures
  const sigs =
    signatures ||
    (
      await conn.getSignaturesForAddress(new PublicKey(programId), {
        limit: 100,
      })
    ).map((s) => s.signature);

  console.log(`[Solana Backfill] Found ${sigs.length} transactions`);

  for (const sig of sigs) {
    try {
      const tx = await conn.getTransaction(sig, {
        commitment: "confirmed",
        maxSupportedTransactionVersion: 0,
      });

      if (tx && tx.meta && tx.meta.logMessages) {
        const hasRegisterToken = tx.meta.logMessages.some(
          (log) =>
            log.includes("Instruction: RegisterToken") ||
            log.includes("register_token"),
        );

        if (hasRegisterToken) {
          await parseRegisterTokenTransaction(tx);
        }
      }
    } catch (error) {
      console.warn(
        `[Solana Backfill] Failed to process signature ${sig}:`,
        error,
      );
    }
  }

  console.log("[Solana Backfill] ✅ Backfill complete");
}

} // namespace elizaos
