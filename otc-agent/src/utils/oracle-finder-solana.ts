// Connection type not currently used but kept for future RPC calls
// import { Connection } from "@solana/web3.js";

export interface SolanaOracleInfo {
  type: "pyth" | "jupiter" | "raydium" | "orca" | "pumpswap";
  address: string;
  feedId?: string; // For Pyth
  poolAddress?: string; // For AMM pools
  liquidity?: number;
  valid: boolean;
  warning?: string;
}

const PYTH_PROGRAM_ID = "FsJ3A3u2vn5cTVofAjvy6y5kwABJAqYWpe4975bi2epH";
const JUPITER_PROGRAM_ID = "JUP6LkbZbjS1jKKwapdHNy74zcZ3tLUZoi5QNyVTaV4";

/**
 * Find price oracle for a Solana token
 * Priority: Pyth > Jupiter > Raydium
 */
export async function findSolanaOracle(
  tokenMint: string,
): Promise<SolanaOracleInfo | null> {
  // Try Pyth first (most reliable)
  const pythFeed = await findPythFeed(tokenMint);
  if (pythFeed) {
    return pythFeed;
  }

  // Try Jupiter aggregator
  const jupiterPool = await findJupiterPool(tokenMint);
  if (jupiterPool) {
    return jupiterPool;
  }

  // Try Raydium pools
  const raydiumPool = await findRaydiumPool(tokenMint);
  if (raydiumPool) {
    return raydiumPool;
  }

  return null;
}

/**
 * Check if token has a Pyth price feed
 * Pyth is the preferred oracle for Solana
 */
async function findPythFeed(
  tokenMint: string,
): Promise<SolanaOracleInfo | null> {
  // Query Pyth API for price feed
  const response = await fetch(
    `https://hermes.pyth.network/api/latest_price_feeds?ids[]=${tokenMint}`,
  );

  if (!response.ok) {
    return null;
  }

  const data = await response.json();

  if (data && data.length > 0) {
    const feed = data[0];
    return {
      type: "pyth",
      address: PYTH_PROGRAM_ID,
      feedId: feed.id,
      valid: true,
    };
  }

  // Also try querying by symbol if mint doesn't work
  // This requires additional metadata lookup
  return null;
}

/**
 * Find Jupiter aggregator pool for token
 */
async function findJupiterPool(
  tokenMint: string,
): Promise<SolanaOracleInfo | null> {
  // Query Jupiter API for available routes
  const response = await fetch(
    `https://quote-api.jup.ag/v6/quote?inputMint=${tokenMint}&outputMint=EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v&amount=1000000`,
  );

  if (!response.ok) {
    return null;
  }

  const data = await response.json();

  if (data && data.routePlan && data.routePlan.length > 0) {
    // Extract pool address from route
    const firstRoute = data.routePlan[0];

    return {
      type: "jupiter",
      address: JUPITER_PROGRAM_ID,
      poolAddress: firstRoute.swapInfo?.ammKey || "unknown",
      valid: true,
    };
  }

  return null;
}

/**
 * Find Raydium pool for token
 */
async function findRaydiumPool(
  tokenMint: string,
): Promise<SolanaOracleInfo | null> {
  // Query Raydium API for pools (RPC connection not needed for API calls)
  const response = await fetch("https://api.raydium.io/v2/main/pairs");

  if (!response.ok) {
    return null;
  }

  interface RaydiumPool {
    baseMint: string;
    quoteMint: string;
    ammId: string;
    liquidity?: string;
  }
  const pools: RaydiumPool[] = await response.json();

  // Find pool with this token
  const pool = pools.find(
    (p) => p.baseMint === tokenMint || p.quoteMint === tokenMint,
  );

  if (pool) {
    // Validate pool has liquidity
    const liquidity = parseFloat(pool.liquidity || "0");

    if (liquidity < 50000) {
      return {
        type: "raydium",
        address: pool.ammId,
        poolAddress: pool.ammId,
        liquidity,
        valid: false,
        warning: `Low liquidity: $${liquidity.toLocaleString()}. Minimum recommended: $50,000`,
      };
    }

    return {
      type: "raydium",
      address: pool.ammId,
      poolAddress: pool.ammId,
      liquidity,
      valid: true,
    };
  }

  return null;
}

/**
 * Validate oracle has sufficient data/liquidity
 */
export function validateSolanaOracle(oracle: SolanaOracleInfo): {
  valid: boolean;
  message: string;
} {
  if (!oracle.valid) {
    return {
      valid: false,
      message: oracle.warning || "Oracle validation failed",
    };
  }

  if (oracle.type === "pyth") {
    return {
      valid: true,
      message: "Pyth price feed available (recommended)",
    };
  }

  if (oracle.type === "jupiter") {
    return {
      valid: true,
      message: "Jupiter aggregator available",
    };
  }

  if (oracle.type === "raydium") {
    if (oracle.liquidity && oracle.liquidity >= 50000) {
      return {
        valid: true,
        message: `Raydium pool available (TVL: $${oracle.liquidity.toLocaleString()})`,
      };
    } else {
      return {
        valid: false,
        message: "Raydium pool liquidity too low",
      };
    }
  }

  if (oracle.type === "orca") {
    if (oracle.liquidity && oracle.liquidity >= 50000) {
      return {
        valid: true,
        message: `Orca Whirlpool available (TVL: $${oracle.liquidity.toLocaleString()})`,
      };
    } else {
      return {
        valid: false,
        message: "Orca pool liquidity too low",
      };
    }
  }

  if (oracle.type === "pumpswap") {
    // PumpSwap/Pump.fun bonding curves typically have lower liquidity but are still valid
    if (oracle.liquidity && oracle.liquidity >= 10000) {
      return {
        valid: true,
        message: `PumpSwap bonding curve available (TVL: $${oracle.liquidity.toLocaleString()})`,
      };
    } else {
      return {
        valid: false,
        message: "PumpSwap bonding curve liquidity too low (min $10k required)",
      };
    }
  }

  return {
    valid: false,
    message: "Unknown oracle type",
  };
}

/**
 * Format oracle info for display
 */
export function formatOracleInfo(oracle: SolanaOracleInfo): string {
  const tvl = oracle.liquidity
    ? ` - TVL: $${oracle.liquidity.toLocaleString()}`
    : "";

  switch (oracle.type) {
    case "pyth":
      return "Pyth Price Feed (Most Reliable)";
    case "jupiter":
      return "Jupiter Aggregator";
    case "raydium":
      return `Raydium Pool${tvl}`;
    case "orca":
      return `Orca Whirlpool${tvl}`;
    case "pumpswap":
      return `PumpSwap Bonding Curve${tvl}`;
    default:
      return "Unknown Oracle";
  }
}

/**
 * Get registration cost estimate for Solana
 */
export function getSolanaRegistrationCost(): {
  sol: number;
  usd: number;
  breakdown: {
    accountCreation: number;
    programExecution: number;
    infrastructure: number;
  };
} {
  return {
    sol: 0.01,
    usd: 2.0, // Approximate at $200 SOL
    breakdown: {
      accountCreation: 0.005, // ~$1
      programExecution: 0.003, // ~$0.60
      infrastructure: 0.002, // ~$0.40
    },
  };
}
