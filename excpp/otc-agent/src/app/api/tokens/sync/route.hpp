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
;
;
;

const ERC20_ABI = parseAbi([
  " = body;

    if (!chain || !transactionHash) {
      return NextResponse.json(
        { success: false, error: "Missing chain or transactionHash" },
        { status: 400 },
      );
    }

    if (chain === "base" || chain === "bsc" || chain === "ethereum") {
      return await syncEvmToken(transactionHash, blockNumber, chain);
    } else if (chain === "solana") {
      return await syncSolanaToken(transactionHash);
    } else {
      return NextResponse.json(
        { success: false, error: "Unsupported chain" },
        { status: 400 },
      );
    }
  } catch (error) {
    console.error("[Sync API] Error:", error);
    return NextResponse.json(
      {
        success: false,
        error: error instanceof Error ? error.message : "Unknown error",
      },
      { status: 500 },
    );
  }
}

/**
 * Sync EVM token registration immediately (Base or BSC)
 */
async  = await import("viem/chains");

  const registrationHelperAddress =
    chain === "bsc"
      ? process.env.NEXT_PUBLIC_BSC_REGISTRATION_HELPER_ADDRESS
      : process.env.NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS;

  if (!registrationHelperAddress) {
    return NextResponse.json(
      {
        success: false,
        error: `REGISTRATION_HELPER_ADDRESS not configured for ${chain}`,
      },
      { status: 500 },
    );
  }

  const rpcUrl =
    chain === "bsc"
      ? process.env.NEXT_PUBLIC_BSC_RPC_URL ||
        "https://bsc-dataseed1.binance.org"
      : process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://mainnet.base.org";

  const viemChain = chain === "bsc" ? bsc : base;
  const client = createPublicClient({
    chain: viemChain,
    transport: http(rpcUrl),
  });

  try {
    // Get transaction receipt to find the block
    const receipt = await client.getTransactionReceipt({
      hash: transactionHash as `0x${string}`,
    });
    if (!receipt) {
      return NextResponse.json(
        { success: false, error: "Transaction not found" },
        { status: 404 },
      );
    }

    const txBlock = receipt.blockNumber;
    const startBlock = blockNumber ? BigInt(blockNumber) : txBlock;
    const endBlock = txBlock;

    console.log(
      `[Sync ${chain.toUpperCase()}] Fetching events from block ${startBlock} to ${endBlock}`,
    );

    // Get logs for this specific transaction
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
          { type: "address", name: "registeredBy", indexed: true },
        ],
      },
      fromBlock: startBlock,
      toBlock: endBlock,
    });

    // Filter logs to only this transaction
    const txLogs = logs.filter(
      (log) => log.transactionHash === transactionHash,
    );

    if (txLogs.length === 0) {
      return NextResponse.json(
        {
          success: false,
          error: "No TokenRegistered event found in transaction",
        },
        { status: 404 },
      );
    }

    let processed = 0;
    const processedTokens: string[] = [];

    for (const log of txLogs) {
      try {
        const { tokenAddress, registeredBy } = log.args as {
          tokenId: string;
          tokenAddress: string;
          pool: string;
          registeredBy: string;
        };

        console.log(
          `[Sync ${chain.toUpperCase()}] Processing token registration: ${tokenAddress} by ${registeredBy}`,
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

        // Register to database - use the chain parameter (base or bsc)
        const tokenService = new TokenRegistryService();
        const dbChain = chain === "bsc" ? "bsc" : "base";
        const token = await tokenService.registerToken({
          symbol: symbol as string,
          name: name as string,
          contractAddress: tokenAddress.toLowerCase(),
          chain: dbChain,
          decimals: Number(decimals),
          logoUrl: undefined,
          description: `Registered via RegistrationHelper by ${registeredBy}`,
        });

        processed++;
        processedTokens.push(token.id);
        console.log(
          `[Sync ${chain.toUpperCase()}] ✅ Registered ${symbol} (${tokenAddress})`,
        );
      } catch (error) {
        console.error(
          `[Sync ${chain.toUpperCase()}] Failed to process event:`,
          error,
        );
      }
    }

    return NextResponse.json({
      success: true,
      processed,
      tokens: processedTokens,
      message: `Successfully synced ${processed} token(s) on ${chain}`,
    });
  } catch (error) {
    console.error(`[Sync ${chain.toUpperCase()}] Error:`, error);
    return NextResponse.json(
      {
        success: false,
        error: error instanceof Error ? error.message : "Unknown error",
      },
      { status: 500 },
    );
  }
}

/**
 * Sync Solana token registration immediately
 */
async ,
      { status: 500 },
    );
  }

  const rpcUrl =
    process.env.NEXT_PUBLIC_SOLANA_RPC || "https://api.mainnet-beta.solana.com";
  const connection = new Connection(rpcUrl, "confirmed");

  try {
    console.log(`[Sync Solana] Fetching transaction: ${signature}`);

    const tx = await connection.getTransaction(signature, {
      commitment: "confirmed",
      maxSupportedTransactionVersion: 0,
    });

    if (!tx) {
      return NextResponse.json(
        { success: false, error: "Transaction not found" },
        { status: 404 },
      );
    }

    if (!tx.meta || !tx.meta.logMessages) {
      return NextResponse.json(
        { success: false, error: "No log messages in transaction" },
        { status: 404 },
      );
    }

    const hasRegisterToken = tx.meta.logMessages.some(
      (log) =>
        log.includes("Instruction: RegisterToken") ||
        log.includes("register_token"),
    );

    if (!hasRegisterToken) {
      return NextResponse.json(
        { success: false, error: "No register_token instruction found" },
        { status: 404 },
      );
    }

    // TODO: Parse Solana transaction to extract token details
    // For now, just acknowledge we found it
    console.log(`[Sync Solana] Detected token registration: ${signature}`);

    return NextResponse.json({
      success: true,
      processed: 0, // Solana parsing not fully implemented yet
      message: "Token registration detected (Solana parsing pending)",
    });
  } catch (error) {
    console.error("[Sync Solana] Error:", error);
    return NextResponse.json(
      {
        success: false,
        error: error instanceof Error ? error.message : "Unknown error",
      },
      { status: 500 },
    );
  }
}

} // namespace elizaos
