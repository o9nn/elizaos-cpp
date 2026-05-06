import { Connection, PublicKey } from "@solana/web3.js";
import { getCached, setCache } from "./retry-cache";

// Cache TTL for Solana pool info (30 seconds)
const SOLANA_POOL_CACHE_TTL_MS = 30_000;

// Rate limiting: delay between sequential RPC calls (ms)
const RPC_CALL_DELAY_MS = 500;

// Helper to check if error is rate limit related
function isRateLimitError(error: unknown): boolean {
  if (error instanceof Error) {
    const message = error.message.toLowerCase();
    return (
      message.includes("429") ||
      message.includes("rate limit") ||
      message.includes("too many requests")
    );
  }
  return false;
}

// Helper to delay between RPC calls
async function delay(ms: number): Promise<void> {
  await new Promise((resolve) => setTimeout(resolve, ms));
}

export interface SolanaPoolInfo {
  protocol: "Raydium" | "Meteora" | "Orca" | "PumpSwap";
  address: string;
  tokenA: string;
  tokenB: string;
  liquidity: number;
  tvlUsd: number;
  priceUsd?: number;
  baseToken: "SOL" | "USDC";
  // PumpSwap-specific vault addresses (for on-chain price updates)
  solVault?: string; // SOL vault account (lamports)
  tokenVault?: string; // Token vault account (SPL tokens)
}

const RAYDIUM_AMM_PROGRAM_MAINNET = new PublicKey(
  "675kPX9MHTjS2zt1qfr1NYHuzeLXfQM9H24wFSUt1Mp8",
);
const RAYDIUM_AMM_PROGRAM_DEVNET = new PublicKey(
  "HWy1jotHpo6UqeQxx49dpYYdQB8wj9Qk9MdxwjLvDHB8",
);

// PumpSwap AMM Program (same for mainnet/devnet)
const PUMPSWAP_AMM_PROGRAM = new PublicKey(
  "pAMMBay6oceH9fJKBRHGP5D4bD4sWpmSwMn52FMfXEA",
);

// Mainnet Mints
const USDC_MINT_MAINNET = new PublicKey(
  "EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v",
);
const SOL_MINT = new PublicKey("So11111111111111111111111111111111111111112");

// Devnet Mints
const USDC_MINT_DEVNET = new PublicKey(
  "4zMMC9srt5Ri5X14GAgXhaHii3GnPAEERYPJgZJDncDU",
);

export async function findBestSolanaPool(
  tokenMint: string,
  cluster: "mainnet" | "devnet" = "mainnet",
  rpcConnection?: Connection,
): Promise<SolanaPoolInfo | null> {
  const cacheKey = `solana-pool:${cluster}:${tokenMint}`;

  // Check cache first
  const cached = getCached<SolanaPoolInfo | null>(cacheKey);
  if (cached !== null) {
    return cached;
  }

  // Use public RPCs that are less restrictive for getProgramAccounts
  const rpcUrl =
    cluster === "mainnet"
      ? "https://api.mainnet-beta.solana.com" // Official public RPC usually allows some GPA
      : "https://api.devnet.solana.com";

  const connection = rpcConnection || new Connection(rpcUrl, "confirmed");
  const mint = new PublicKey(tokenMint);

  let pumpSwapPools: SolanaPoolInfo[] = [];
  let raydiumPools: SolanaPoolInfo[] = [];

  // Strategy: Use Sequential execution by default to avoid 429 rate limits
  // Public RPCs are very restrictive, so sequential is more reliable
  try {
    // Try PumpSwap first
    pumpSwapPools = await findPumpSwapPools(connection, mint, cluster, false);
    // Delay between calls to respect rate limits
    await delay(RPC_CALL_DELAY_MS);
  } catch (err) {
    if (isRateLimitError(err)) {
      console.warn(
        "PumpSwap pool discovery rate limited, waiting before retry...",
      );
      await delay(2000); // Wait 2s on rate limit
      try {
        pumpSwapPools = await findPumpSwapPools(
          connection,
          mint,
          cluster,
          false,
        );
      } catch (e) {
        console.warn("PumpSwap discovery failed after retry", e);
      }
    } else {
      console.warn("PumpSwap pool discovery failed:", err);
    }
  }

  try {
    // Then try Raydium
    await delay(RPC_CALL_DELAY_MS);
    raydiumPools = await findRaydiumPools(connection, mint, cluster, false);
  } catch (err) {
    if (isRateLimitError(err)) {
      console.warn(
        "Raydium pool discovery rate limited, waiting before retry...",
      );
      await delay(2000); // Wait 2s on rate limit
      try {
        raydiumPools = await findRaydiumPools(connection, mint, cluster, false);
      } catch (e) {
        console.warn("Raydium discovery failed after retry", e);
      }
    } else {
      console.warn("Raydium pool discovery failed:", err);
    }
  }

  const allPools = [...pumpSwapPools, ...raydiumPools];

  if (allPools.length === 0) {
    // Cache null result too
    setCache(cacheKey, null, SOLANA_POOL_CACHE_TTL_MS);
    return null;
  }

  // Sort by TVL descending
  allPools.sort((a, b) => b.tvlUsd - a.tvlUsd);

  const bestPool = allPools[0];
  setCache(cacheKey, bestPool, SOLANA_POOL_CACHE_TTL_MS);
  return bestPool;
}

async function findPumpSwapPools(
  connection: Connection,
  mint: PublicKey,
  cluster: "mainnet" | "devnet",
  strict: boolean = false,
): Promise<SolanaPoolInfo[]> {
  const pools: SolanaPoolInfo[] = [];
  const USDC_MINT =
    cluster === "mainnet" ? USDC_MINT_MAINNET : USDC_MINT_DEVNET;

  try {
    // PumpSwap pools use memcmp filters at offsets 43 (base_mint) and 75 (quote_mint)
    // Based on: https://github.com/AL-THE-BOT-FATHER/pump_swap_market_cap
    const mintBytes = mint.toBase58();

    // Try both directions: token as base, and token as quote
    const filtersBase = [
      { memcmp: { offset: 43, bytes: mintBytes } },
      { memcmp: { offset: 75, bytes: SOL_MINT.toBase58() } },
    ];

    const filtersQuote = [
      { memcmp: { offset: 75, bytes: mintBytes } },
      { memcmp: { offset: 43, bytes: SOL_MINT.toBase58() } },
    ];

    // Run sequentially to avoid rate limits
    type ProgramAccount = Awaited<
      ReturnType<typeof connection.getProgramAccounts>
    >[number];

    let poolsBase: readonly ProgramAccount[] = [];
    let poolsQuote: readonly ProgramAccount[] = [];

    try {
      poolsBase = await connection.getProgramAccounts(PUMPSWAP_AMM_PROGRAM, {
        filters: filtersBase,
      });
      await delay(RPC_CALL_DELAY_MS);
    } catch (e) {
      if (strict) throw e;
      console.warn("PumpSwap base filter failed:", e);
    }

    try {
      poolsQuote = await connection.getProgramAccounts(PUMPSWAP_AMM_PROGRAM, {
        filters: filtersQuote,
      });
    } catch (e) {
      if (strict) throw e;
      console.warn("PumpSwap quote filter failed:", e);
    }

    const all = [
      ...(Array.isArray(poolsBase) ? poolsBase : []),
      ...(Array.isArray(poolsQuote) ? poolsQuote : []),
    ];

    // Process results
    for (const account of all) {
      try {
        const data = account.account.data;
        const readPubkey = (offset: number) =>
          new PublicKey(data.subarray(offset, offset + 32));

        // PumpSwap pool layout (from Python code):
        // offset 43: base_mint
        // offset 75: quote_mint
        // offset 139: pool_base_token_account
        // offset 171: pool_quote_token_account
        const baseMint = readPubkey(43);
        const quoteMint = readPubkey(75);
        const poolBaseTokenAccount = readPubkey(139);
        const poolQuoteTokenAccount = readPubkey(171);

        // PumpSwap typically pairs with WSOL (SOL)
        let baseToken: "SOL" | "USDC" | null = null;
        let otherMint: PublicKey | null = null;

        if (quoteMint.equals(USDC_MINT) || baseMint.equals(USDC_MINT)) {
          baseToken = "USDC";
          otherMint = baseMint.equals(USDC_MINT) ? quoteMint : baseMint;
        } else if (quoteMint.equals(SOL_MINT) || baseMint.equals(SOL_MINT)) {
          baseToken = "SOL";
          otherMint = baseMint.equals(SOL_MINT) ? quoteMint : baseMint;
        }

        if (baseToken && otherMint && otherMint.equals(mint)) {
          // Get token account balances sequentially to avoid rate limits
          let baseBalance = { value: { uiAmount: 0 } };
          let quoteBalance = { value: { uiAmount: 0 } };

          try {
            baseBalance =
              await connection.getTokenAccountBalance(poolBaseTokenAccount);
            await delay(RPC_CALL_DELAY_MS);
          } catch {
            // Ignore errors, use default
          }

          try {
            quoteBalance = await connection.getTokenAccountBalance(
              poolQuoteTokenAccount,
            );
            await delay(RPC_CALL_DELAY_MS);
          } catch {
            // Ignore errors, use default
          }

          const baseAmount = baseBalance.value.uiAmount || 0;
          const quoteAmount = quoteBalance.value.uiAmount || 0;

          // Determine SOL/USDC amount based on which mint is the base token
          // pool_base_token_account holds base_mint, pool_quote_token_account holds quote_mint
          const solOrUsdcAmount =
            baseMint.equals(SOL_MINT) || baseMint.equals(USDC_MINT)
              ? baseAmount // base_mint is SOL/USDC, so poolBaseTokenAccount holds it
              : quoteAmount; // quote_mint is SOL/USDC, so poolQuoteTokenAccount holds it

          // Calculate TVL: for SOL pairs, use SOL amount * price estimate * 2 (both sides of pool)
          // For USDC pairs, use USDC amount * 2
          const solPriceEstimate = 240; // Estimate (updated Nov 2025)

          const tvlUsd =
            baseToken === "USDC"
              ? solOrUsdcAmount * 2 // USDC is 1:1 USD, pool has equal value on both sides
              : solOrUsdcAmount * solPriceEstimate * 2; // SOL price estimate ~$200, pool has equal value on both sides

          // Calculate Spot Price
          // Price = QuoteAmount / BaseAmount
          // Adjust for decimals?
          // We have amounts in UI (decimals already handled by getParsedTokenAccounts? No, getTokenAccountBalance returns uiAmount)
          // Yes, uiAmount handles decimals.

          let priceUsd = 0;
          if (baseToken === "USDC") {
            // Quote is USDC. Price = USDC / Token
            // BaseAmount is Token amount? No.
            // We need amount of Token.
            // if baseToken is USDC, then 'solOrUsdcAmount' is the USDC amount.
            // The OTHER token is the target token.
            // We need the amount of the OTHER token.

            const tokenAmount = baseMint.equals(USDC_MINT)
              ? quoteAmount
              : baseAmount;
            // Price = USDC / Token (with division by zero protection)
            priceUsd = tokenAmount > 0 ? solOrUsdcAmount / tokenAmount : 0;
          } else {
            // Base is SOL. Price = SOL / Token * SolPrice
            const tokenAmount = baseMint.equals(SOL_MINT)
              ? quoteAmount
              : baseAmount;
            priceUsd =
              tokenAmount > 0
                ? (solOrUsdcAmount / tokenAmount) * solPriceEstimate
                : 0;
          }

          // Determine which vault is SOL and which is token
          // If baseMint is SOL/USDC, then poolBaseTokenAccount holds SOL/USDC
          const isSolBase = baseMint.equals(SOL_MINT);
          const isUsdcBase = baseMint.equals(USDC_MINT);

          // For price updates, we need the SOL vault and token vault
          // SOL vault: holds the SOL (lamports) - for SOL pairs
          // Token vault: holds the SPL tokens
          const solVault = isSolBase
            ? poolBaseTokenAccount.toBase58()
            : quoteMint.equals(SOL_MINT)
              ? poolQuoteTokenAccount.toBase58()
              : undefined;
          const tokenVault =
            isSolBase || isUsdcBase
              ? poolQuoteTokenAccount.toBase58()
              : poolBaseTokenAccount.toBase58();

          pools.push({
            protocol: "PumpSwap",
            address: account.pubkey.toBase58(),
            tokenA: baseMint.toBase58(),
            tokenB: quoteMint.toBase58(),
            liquidity: solOrUsdcAmount, // Base token (SOL/USDC) liquidity amount
            tvlUsd,
            priceUsd,
            baseToken,
            // PumpSwap-specific vault addresses for on-chain price updates
            solVault,
            tokenVault,
          });
        }
      } catch {
        // Skip invalid pools
        continue;
      }
    }
  } catch (err: unknown) {
    if (strict) throw err;
    // Handle RPC errors gracefully (e.g., 429 Too Many Requests, secondary index limitations)
    // Return empty array to allow other pool finders to proceed
    console.warn("PumpSwap pool discovery failed:", err);
    return [];
  }

  return pools;
}

async function findRaydiumPools(
  connection: Connection,
  mint: PublicKey,
  cluster: "mainnet" | "devnet",
  strict: boolean = false,
): Promise<SolanaPoolInfo[]> {
  const pools: SolanaPoolInfo[] = [];
  const PROGRAM_ID =
    cluster === "mainnet"
      ? RAYDIUM_AMM_PROGRAM_MAINNET
      : RAYDIUM_AMM_PROGRAM_DEVNET;
  const USDC_MINT =
    cluster === "mainnet" ? USDC_MINT_MAINNET : USDC_MINT_DEVNET;

  // Alternative Strategy: Fetch multiple accounts in a batch if possible, or use getProgramAccounts with stricter filters
  // The public RPC nodes often block getProgramAccounts for large datasets like Raydium.
  // However, filtering by size (752) AND memcmp (mint at offset) is usually allowed as it's efficient.

  const filtersBase = [
    { dataSize: 752 },
    { memcmp: { offset: 400, bytes: mint.toBase58() } },
  ];

  const filtersQuote = [
    { dataSize: 752 },
    { memcmp: { offset: 432, bytes: mint.toBase58() } },
  ];

  // Run sequentially to avoid rate limits
  type RaydiumProgramAccount = Awaited<
    ReturnType<typeof connection.getProgramAccounts>
  >[number];

  let poolsBase: readonly RaydiumProgramAccount[] = [];
  let poolsQuote: readonly RaydiumProgramAccount[] = [];

  try {
    poolsBase = await connection.getProgramAccounts(PROGRAM_ID, {
      filters: filtersBase,
    });
    await delay(RPC_CALL_DELAY_MS);
  } catch (e) {
    if (strict) throw e;
    console.warn("Raydium base filter failed:", e);
  }

  try {
    poolsQuote = await connection.getProgramAccounts(PROGRAM_ID, {
      filters: filtersQuote,
    });
  } catch (e) {
    if (strict) throw e;
    console.warn("Raydium quote filter failed:", e);
  }

  const all = [
    ...(Array.isArray(poolsBase) ? poolsBase : []),
    ...(Array.isArray(poolsQuote) ? poolsQuote : []),
  ];

  // Process results (same as before)
  for (const account of all) {
    const data = account.account.data;
    const readPubkey = (offset: number) =>
      new PublicKey(data.subarray(offset, offset + 32));

    const coinMint = readPubkey(400);
    const pcMint = readPubkey(432);

    let baseToken: "SOL" | "USDC" | null = null;
    let otherMint: PublicKey | null = null;

    if (coinMint.equals(USDC_MINT) || pcMint.equals(USDC_MINT)) {
      baseToken = "USDC";
      otherMint = coinMint.equals(USDC_MINT) ? pcMint : coinMint;
    } else if (coinMint.equals(SOL_MINT) || pcMint.equals(SOL_MINT)) {
      baseToken = "SOL";
      otherMint = coinMint.equals(SOL_MINT) ? pcMint : coinMint;
    }

    if (baseToken && otherMint && otherMint.equals(mint)) {
      const coinVault = readPubkey(496);
      const pcVault = readPubkey(528);

      const vaultToCheck =
        baseToken === "USDC"
          ? coinMint.equals(USDC_MINT)
            ? coinVault
            : pcVault
          : coinMint.equals(SOL_MINT)
            ? coinVault
            : pcVault;

      let balance = { value: { uiAmount: 0 } };
      try {
        balance = await connection.getTokenAccountBalance(vaultToCheck);
        await delay(RPC_CALL_DELAY_MS);
      } catch {
        // Ignore errors, use default
      }
      const amount = balance.value.uiAmount || 0;

      const tvlUsd = baseToken === "USDC" ? amount * 2 : amount * 240 * 2; // SOL ~$240

      // Calculate Price for Raydium
      // We only have 'amount' of Base Token (USDC/SOL) from 'vaultToCheck'.
      // We need amount of Token Vault to calculate price.
      // But we didn't fetch it. 'findRaydiumPools' only fetches 'vaultToCheck'.

      // Optimization: To get price, we must fetch the other vault too.
      // This adds RPC calls.
      // We can fetch it in parallel?
      // Or leave priceUsd undefined for Raydium if we want to save calls, but then validation fails.
      // Let's fetch it.

      let priceUsd = 0;
      const otherVault =
        baseToken === "USDC"
          ? coinMint.equals(USDC_MINT)
            ? pcVault
            : coinVault
          : coinMint.equals(SOL_MINT)
            ? pcVault
            : coinVault;
      let otherBalance = { value: { uiAmount: 0 } };
      try {
        otherBalance = await connection.getTokenAccountBalance(otherVault);
        await delay(RPC_CALL_DELAY_MS);
      } catch {
        // Ignore errors, use default
      }
      const otherAmount = otherBalance.value.uiAmount || 0;

      if (otherAmount > 0) {
        const solPriceEstimate = 240; // Estimate (updated Nov 2025)
        if (baseToken === "USDC") {
          priceUsd = amount / otherAmount;
        } else {
          priceUsd = (amount / otherAmount) * solPriceEstimate;
        }
      }

      pools.push({
        protocol: "Raydium",
        address: account.pubkey.toBase58(),
        tokenA: coinMint.toBase58(),
        tokenB: pcMint.toBase58(),
        liquidity: amount,
        tvlUsd,
        priceUsd,
        baseToken,
      });
    }
  }

  return pools;
}

// async function findMeteoraPools(
//   connection: Connection,
//   mint: PublicKey
// ): Promise<SolanaPoolInfo[]> {
//   return [];
// }
