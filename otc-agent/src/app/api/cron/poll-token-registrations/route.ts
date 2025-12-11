import { NextRequest, NextResponse } from "next/server";
import { createPublicClient, http, parseAbi } from "viem";
import { base } from "viem/chains";
import { Connection, PublicKey } from "@solana/web3.js";
import { TokenRegistryService } from "@/services/tokenRegistry";

const ERC20_ABI = parseAbi([
  "function symbol() view returns (string)",
  "function name() view returns (string)",
  "function decimals() view returns (uint8)",
]);

// Store last processed state
// Note: In-memory state resets on each Vercel function invocation
// For production, consider using Vercel KV or database for persistence
// Current implementation checks last 1000 blocks each run (safe for 2-min intervals)
let lastBaseBlock: bigint | null = null;
let lastSolanaSignature: string | null = null;

// Try to load from environment (set via Vercel KV or external service if needed)
function getLastBaseBlock(): bigint | null {
  const envBlock = process.env.LAST_PROCESSED_BASE_BLOCK;
  if (envBlock) {
    return BigInt(envBlock);
  }
  return lastBaseBlock;
}

/**
 * Poll for new token registrations (Base)
 */
async function pollBaseRegistrations() {
  const registrationHelperAddress =
    process.env.NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS;
  if (!registrationHelperAddress) {
    console.error("[Cron] REGISTRATION_HELPER_ADDRESS not configured");
    return {
      processed: 0,
      error: "REGISTRATION_HELPER_ADDRESS not configured",
    };
  }

  const rpcUrl =
    process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://mainnet.base.org";
  const client = createPublicClient({
    chain: base,
    transport: http(rpcUrl),
  });

  try {
    const latestBlock = await client.getBlockNumber();

    // If we don't have a last block, start from 1000 blocks ago (to catch up)
    // This ensures we don't miss events even if state resets
    const savedBlock = getLastBaseBlock();
    const startBlock = savedBlock || latestBlock - BigInt(1000);

    // Don't process if we're already up to date
    if (startBlock >= latestBlock) {
      return { processed: 0, message: "Already up to date" };
    }

    console.log(
      `[Cron Base] Fetching events from block ${startBlock} to ${latestBlock}`,
    );

    const logs = await client.getLogs({
      address: registrationHelperAddress as `0x${string}`,
      event: {
        type: "event",
        name: "TokenRegistered",
        inputs: [
          { type: "bytes32", name: "tokenId", indexed: true },
          { type: "address", name: "tokenAddress", indexed: true },
          { type: "address", name: "pool", indexed: true },
          { type: "address", name: "oracle" },
          { type: "address", name: "registeredBy" },
        ],
      },
      fromBlock: startBlock,
      toBlock: latestBlock,
    });

    console.log(`[Cron Base] Found ${logs.length} TokenRegistered events`);

    let processed = 0;
    for (const log of logs) {
      const { tokenAddress, registeredBy } = log.args as {
        tokenId: string;
        tokenAddress: string;
        pool: string;
        registeredBy: string;
      };

      console.log(
        `[Cron Base] Processing token registration: ${tokenAddress} by ${registeredBy}`,
      );

      // Fetch token metadata
      // Use type assertion to bypass viem's strict authorizationList requirement
      const readContract = client.readContract as (
        params: unknown,
      ) => Promise<unknown>;
      const [symbol, name, decimals] = await Promise.all([
        readContract({
          address: tokenAddress as `0x${string}`,
          abi: ERC20_ABI,
          functionName: "symbol",
        }),
        readContract({
          address: tokenAddress as `0x${string}`,
          abi: ERC20_ABI,
          functionName: "name",
        }),
        readContract({
          address: tokenAddress as `0x${string}`,
          abi: ERC20_ABI,
          functionName: "decimals",
        }),
      ]);

      // Register to database
      const tokenService = new TokenRegistryService();
      await tokenService.registerToken({
        symbol: symbol as string,
        name: name as string,
        contractAddress: tokenAddress.toLowerCase(),
        chain: "base",
        decimals: Number(decimals),
        logoUrl: undefined,
        description: `Registered via RegistrationHelper by ${registeredBy}`,
      });

      processed++;
      console.log(`[Cron Base] ✅ Registered ${symbol} (${tokenAddress})`);
    }

    // Update last processed block
    lastBaseBlock = latestBlock;

    return { processed, latestBlock: latestBlock.toString() };
  } catch (error) {
    console.error("[Cron Base] Error:", error);
    return {
      processed: 0,
      error: error instanceof Error ? error.message : "Unknown error",
    };
  }
}

/**
 * Poll for new token registrations (Solana)
 */
async function pollSolanaRegistrations() {
  const programId = process.env.NEXT_PUBLIC_SOLANA_PROGRAM_ID;
  if (!programId) {
    console.error("[Cron] SOLANA_PROGRAM_ID not configured");
    return { processed: 0, error: "SOLANA_PROGRAM_ID not configured" };
  }

  const rpcUrl =
    process.env.NEXT_PUBLIC_SOLANA_RPC || "https://api.mainnet-beta.solana.com";
  const connection = new Connection(rpcUrl, "confirmed");

  try {
    // Get recent signatures for the program
    const signatures = await connection.getSignaturesForAddress(
      new PublicKey(programId),
      { limit: 50 }, // Check last 50 transactions
    );

    if (signatures.length === 0) {
      return { processed: 0, message: "No recent transactions" };
    }

    // If we have a last signature, only process newer ones
    let startIndex = 0;
    if (lastSolanaSignature) {
      const lastIndex = signatures.findIndex(
        (sig) => sig.signature === lastSolanaSignature,
      );
      if (lastIndex >= 0) {
        startIndex = lastIndex + 1;
      }
    }

    if (startIndex >= signatures.length) {
      return { processed: 0, message: "Already up to date" };
    }

    console.log(
      `[Cron Solana] Checking ${signatures.length - startIndex} transactions`,
    );

    let processed = 0;
    let lastProcessedSig: string | null = null;

    for (let i = startIndex; i < signatures.length; i++) {
      const sig = signatures[i];
      const tx = await connection.getTransaction(sig.signature, {
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
          console.log(
            `[Cron Solana] Detected token registration: ${sig.signature}`,
          );
          // TODO: Parse and register token (Solana parsing not fully implemented)
          // For now, just log it
          processed++;
          lastProcessedSig = sig.signature;
        }
      }
    }

    if (lastProcessedSig) {
      lastSolanaSignature = lastProcessedSig;
    }

    return { processed, lastSignature: lastSolanaSignature };
  } catch (error) {
    console.error("[Cron Solana] Error:", error);
    return {
      processed: 0,
      error: error instanceof Error ? error.message : "Unknown error",
    };
  }
}

/**
 * Vercel Cron Job Handler
 * Runs periodically to poll for new token registrations
 */
export async function GET(request: NextRequest) {
  // Verify authorization
  const authHeader = request.headers.get("authorization");
  const cronSecret = process.env.CRON_SECRET;

  // Always require auth in production
  if (process.env.NODE_ENV === "production" && !cronSecret) {
    console.error("[Cron] No CRON_SECRET configured in production");
    return NextResponse.json(
      { error: "Server configuration error" },
      { status: 500 },
    );
  }

  if (cronSecret && authHeader !== `Bearer ${cronSecret}`) {
    console.warn("[Cron] Unauthorized access attempt", {
      ip:
        request.headers.get("x-forwarded-for") ||
        request.headers.get("x-real-ip"),
      timestamp: new Date().toISOString(),
    });
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  console.log("[Cron] Starting token registration poll...");

  const results = {
    base: {
      processed: 0,
      error: null as string | null,
      latestBlock: null as string | null,
    },
    solana: {
      processed: 0,
      error: null as string | null,
      lastSignature: null as string | null,
    },
    timestamp: new Date().toISOString(),
  };

  // Poll Base
  try {
    const baseResult = await pollBaseRegistrations();
    results.base = {
      processed: baseResult.processed || 0,
      error: baseResult.error || null,
      latestBlock: baseResult.latestBlock || null,
    };
  } catch (error) {
    results.base.error =
      error instanceof Error ? error.message : "Unknown error";
  }

  // Poll Solana
  try {
    const solanaResult = await pollSolanaRegistrations();
    results.solana = {
      processed: solanaResult.processed || 0,
      error: solanaResult.error || null,
      lastSignature: solanaResult.lastSignature || null,
    };
  } catch (error) {
    results.solana.error =
      error instanceof Error ? error.message : "Unknown error";
  }

  const totalProcessed = results.base.processed + results.solana.processed;

  return NextResponse.json({
    success: true,
    message: `Processed ${totalProcessed} new token registrations`,
    results,
  });
}
