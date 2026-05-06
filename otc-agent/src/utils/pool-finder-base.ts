import {
  createPublicClient,
  http,
  parseAbi,
  type PublicClient,
  type Abi,
  type Address,
} from "viem";
import { base, baseSepolia, bsc, bscTestnet } from "viem/chains";
import { getCached, setCache, withRetryAndCache } from "./retry-cache";

/**
 * Type-safe wrapper for readContract calls in this module.
 * viem 2.40+ has strict generics that require compile-time ABI inference.
 * This wrapper allows us to use dynamic ABIs while maintaining type safety on return values.
 */
async function poolReadContract<T>(
  client: PublicClient,
  params: {
    address: Address;
    abi: Abi;
    functionName: string;
    args?: readonly unknown[];
  },
): Promise<T> {
  // Cast is necessary for dynamic ABIs - viem's generics require compile-time inference
  type ReadContractParams = Parameters<typeof client.readContract>[0];
  return client.readContract(params as ReadContractParams) as Promise<T>;
}

// Cache TTL for pool info (30 seconds)
const POOL_CACHE_TTL_MS = 30_000;

// Configuration by Chain ID
const CONFIG: Record<
  number,
  {
    uniV3Factory: string;
    aerodromeFactory?: string;
    aerodromeCLFactory?: string;
    pancakeswapFactory?: string;
    usdc: string;
    weth: string;
    rpcUrl: string;
    nativeToken: string;
    nativeTokenPriceEstimate: number; // For TVL estimation
  }
> = {
  // Base Mainnet
  8453: {
    uniV3Factory: "0x33128a8fC17869897dcE68Ed026d694621f6FDfD",
    aerodromeFactory: "0x420DD381b31aEf6683db6B902084cB0FFECe40Da",
    // Aerodrome Slipstream (Velodrome CL) PoolFactory - verified from official deployment
    // https://github.com/velodrome-finance/slipstream
    aerodromeCLFactory: "0x5e7BB104d84c7CB9B682AaC2F3d509f5F406809A",
    usdc: "0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913",
    weth: "0x4200000000000000000000000000000000000006",
    // Fallback RPC for Base Mainnet to avoid rate limits on public endpoint
    rpcUrl: process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://base.llamarpc.com",
    nativeToken: "ETH",
    nativeTokenPriceEstimate: 3200,
  },
  // Base Sepolia
  84532: {
    uniV3Factory: "0x4752ba5DBc23f44D87826276BF6Fd6b1C372aD24", // Official Uniswap V3 Factory on Base Sepolia
    // Aerodrome not officially on Sepolia, using same address will fail, so undefined
    usdc: "0x036CbD53842c5426634e7929541eC2318f3dCF7e", // Base Sepolia USDC
    weth: "0x4200000000000000000000000000000000000006", // Base Sepolia WETH
    rpcUrl: "https://sepolia.base.org",
    nativeToken: "ETH",
    nativeTokenPriceEstimate: 3200,
  },
  // BSC Mainnet
  56: {
    uniV3Factory: "0xdB1d10011AD0Ff90774D0C6Bb92e5C5c8b4461F7", // Uniswap V3 Factory
    pancakeswapFactory: "0x0BFbCF9fa4f9C56B0F40a671Ad40E0805A091865", // PancakeSwap V3 Factory
    usdc: "0x8AC76a51cc950d9822D68b83fE1Ad97B32Cd580d", // Binance-Peg USDC
    weth: "0xbb4CdB9CBd36B01bD1cBaEBF2De08d9173bc095c", // WBNB
    rpcUrl:
      process.env.NEXT_PUBLIC_BSC_RPC_URL ||
      "https://bsc-dataseed1.binance.org",
    nativeToken: "BNB",
    nativeTokenPriceEstimate: 650,
  },
  // BSC Testnet
  97: {
    uniV3Factory: "0x0BFbCF9fa4f9C56B0F40a671Ad40E0805A091865", // PancakeSwap V3 Factory (Uniswap forks often share addresses on testnets or use same factory logic)
    pancakeswapFactory: "0x0BFbCF9fa4f9C56B0F40a671Ad40E0805A091865",
    usdc: "0x64544969ed7EBf5f083679233325356EbE738930", // USDC on BSC Testnet
    weth: "0xae13d989daC2f0dEbFf460aC112a837C89BAa7cd", // WBNB on BSC Testnet
    rpcUrl: "https://data-seed-prebsc-1-s1.binance.org:8545",
    nativeToken: "BNB",
    nativeTokenPriceEstimate: 650,
  },
};

// Uniswap: 100 (0.01%), 500 (0.05%), 3000 (0.3%), 10000 (1%)
// PancakeSwap: 100 (0.01%), 500 (0.05%), 2500 (0.25%), 10000 (1%)
// Aerodrome CL: Uses tickSpacing instead of fee. Common: 1, 50, 100, 200, 2000
const FEE_TIERS = [100, 500, 2500, 3000, 10000];
const TICK_SPACINGS = [1, 50, 100, 200, 2000];

export interface PoolInfo {
  protocol:
    | "Uniswap V3"
    | "Aerodrome"
    | "Aerodrome Slipstream"
    | "Pancakeswap V3";
  address: string;
  token0: string;
  token1: string;
  fee?: number; // Only for Uniswap V3 / Pancake V3
  tickSpacing?: number; // Only for Aerodrome Slipstream
  stable?: boolean; // Only for Aerodrome V2
  liquidity: bigint;
  tvlUsd: number;
  priceUsd?: number; // Estimated price in USD
  baseToken: "USDC" | "WETH";
}

const uniFactoryAbi = parseAbi([
  "function getPool(address tokenA, address tokenB, uint24 fee) external view returns (address pool)",
]);

const aeroFactoryAbi = parseAbi([
  "function getPool(address tokenA, address tokenB, bool stable) external view returns (address pool)",
]);

const aeroCLFactoryAbi = parseAbi([
  "function getPool(address tokenA, address tokenB, int24 tickSpacing) external view returns (address pool)",
]);

const erc20Abi = parseAbi([
  "function decimals() external view returns (uint8)",
]);

const uniPoolAbi = parseAbi([
  "function token0() external view returns (address)",
  "function token1() external view returns (address)",
  "function liquidity() external view returns (uint128)",
  "function slot0() external view returns (uint160 sqrtPriceX96, int24 tick, uint16 observationIndex, uint16 observationCardinality, uint16 observationCardinalityNext, uint8 feeProtocol, bool unlocked)",
]);

const aeroPoolAbi = parseAbi([
  "function token0() external view returns (address)",
  "function token1() external view returns (address)",
  "function reserve0() external view returns (uint256)",
  "function reserve1() external view returns (uint256)",
  "function totalSupply() external view returns (uint256)",
  "function symbol() external view returns (string)",
]);

/**
 * Find all pools (Uniswap V3 + Aerodrome) for a given token
 * @param tokenAddress The token to find pools for
 * @param chainId The chain ID to search on (default: Base Mainnet 8453)
 * @returns Array of pool information sorted by TVL
 */
export async function findBestPool(
  tokenAddress: string,
  chainId: number = 8453,
): Promise<PoolInfo | null> {
  const cacheKey = `pool:${chainId}:${tokenAddress.toLowerCase()}`;

  // Check cache first
  const cached = getCached<PoolInfo | null>(cacheKey);
  if (cached !== undefined) {
    return cached;
  }

  const config = CONFIG[chainId];
  if (!config) throw new Error(`Unsupported chain ID: ${chainId}`);

  // Determine chain based on chainId
  const chain =
    chainId === 84532
      ? baseSepolia
      : chainId === 56
        ? bsc
        : chainId === 97
          ? bscTestnet
          : base;

  // Create client with explicit type to avoid deep type instantiation
  const client = createPublicClient({
    chain,
    transport: http(config.rpcUrl),
  }) as unknown as PublicClient;

  const promises = [findUniswapV3Pools(client, tokenAddress, config)];

  // Aerodrome Slipstream (CL) pools - compatible with SimplePoolOracle (Uniswap V3 interface)
  // Note: Aerodrome V2 pools (Basic/Volatile) do NOT support the IUniswapV3Pool interface
  if (config.aerodromeCLFactory) {
    promises.push(findAerodromeCLPools(client, tokenAddress, config));
  }

  if (config.pancakeswapFactory) {
    promises.push(findPancakeswapPools(client, tokenAddress, config));
  }

  const results = await Promise.all(promises);
  const allPools = results.flat();

  if (allPools.length === 0) {
    // Cache null result too
    setCache(cacheKey, null, POOL_CACHE_TTL_MS);
    return null;
  }

  // Sort by TVL descending
  allPools.sort((a, b) => b.tvlUsd - a.tvlUsd);

  // Return pool with highest TVL
  const bestPool = allPools[0];
  setCache(cacheKey, bestPool, POOL_CACHE_TTL_MS);
  return bestPool;
}

/**
 * Find Uniswap V3 pools with retry and caching
 */
async function findUniswapV3Pools(
  client: PublicClient,
  tokenAddress: string,
  config: (typeof CONFIG)[number],
): Promise<PoolInfo[]> {
  const pools: PoolInfo[] = [];

  // Helper to check a pool with retry
  const checkPool = async (
    baseTokenAddress: string,
    baseTokenSymbol: "USDC" | "WETH",
    fee: number,
  ) => {
    try {
      const poolAddress = await withRetryAndCache(
        `uni-v3-pool:${config.uniV3Factory}:${tokenAddress}:${baseTokenAddress}:${fee}`,
        async () => {
          return poolReadContract<Address>(client, {
            address: config.uniV3Factory as Address,
            abi: uniFactoryAbi as Abi,
            functionName: "getPool",
            args: [
              tokenAddress as `0x${string}`,
              baseTokenAddress as `0x${string}`,
              fee,
            ],
          });
        },
        { cacheTtlMs: POOL_CACHE_TTL_MS },
      );

      if (
        poolAddress &&
        poolAddress !== "0x0000000000000000000000000000000000000000"
      ) {
        const poolInfo = await getUniPoolInfo(
          client,
          poolAddress,
          baseTokenSymbol,
          fee,
          config,
          tokenAddress,
        );
        if (poolInfo) pools.push(poolInfo);
      }
    } catch {
      // Silently ignore pool check errors (pool doesn't exist or RPC error)
    }
  };

  // Check all combinations
  // Only check USDC if NOT on BSC, because BSC USDC is 18 decimals and breaks SimplePoolOracle
  if (config.nativeToken !== "BNB") {
    await Promise.all([
      ...FEE_TIERS.map((fee) => checkPool(config.usdc, "USDC", fee)),
      ...FEE_TIERS.map((fee) => checkPool(config.weth, "WETH", fee)),
    ]);
  } else {
    // BSC: Only check WETH (WBNB)
    await Promise.all([
      ...FEE_TIERS.map((fee) => checkPool(config.weth, "WETH", fee)),
    ]);
  }

  return pools;
}

/**
 * Find Aerodrome Slipstream (CL) pools
 * These are compatible with SimplePoolOracle (Uniswap V3 interface)
 * Uses the official Velodrome Slipstream PoolFactory
 */
async function findAerodromeCLPools(
  client: PublicClient,
  tokenAddress: string,
  config: (typeof CONFIG)[number],
): Promise<PoolInfo[]> {
  if (!config.aerodromeCLFactory) return [];

  const pools: PoolInfo[] = [];

  // Helper to check a pool with retry
  const checkPool = async (
    baseTokenAddress: string,
    baseTokenSymbol: "USDC" | "WETH",
    tickSpacing: number,
  ) => {
    try {
      const poolAddress = await withRetryAndCache(
        `aero-cl-pool:${config.aerodromeCLFactory}:${tokenAddress}:${baseTokenAddress}:${tickSpacing}`,
        async () => {
          return poolReadContract<Address>(client, {
            address: config.aerodromeCLFactory as Address,
            abi: aeroCLFactoryAbi as Abi,
            functionName: "getPool",
            args: [
              tokenAddress as Address,
              baseTokenAddress as Address,
              tickSpacing,
            ],
          });
        },
        { cacheTtlMs: POOL_CACHE_TTL_MS },
      );

      if (
        poolAddress &&
        poolAddress !== "0x0000000000000000000000000000000000000000"
      ) {
        // Use getUniPoolInfo because CL pools share the same interface as Uni V3
        const poolInfo = await getUniPoolInfo(
          client,
          poolAddress,
          baseTokenSymbol,
          0,
          config,
          tokenAddress,
        );
        if (poolInfo) {
          poolInfo.protocol = "Aerodrome Slipstream";
          poolInfo.tickSpacing = tickSpacing;
          delete poolInfo.fee; // CL pools don't use fee tier in same way (fee is dynamic or based on tickSpacing)
          pools.push(poolInfo);
        }
      }
    } catch {
      // Silently ignore pool check errors (pool doesn't exist)
    }
  };

  // Check all combinations
  // On Base, check both USDC and WETH
  await Promise.all([
    ...TICK_SPACINGS.map((ts) => checkPool(config.usdc, "USDC", ts)),
    ...TICK_SPACINGS.map((ts) => checkPool(config.weth, "WETH", ts)),
  ]);

  return pools;
}

/**
 * Find Aerodrome pools (Stable + Volatile)
 * @internal Currently disabled pending token-agnostic deployment
 */
// eslint-disable-next-line @typescript-eslint/no-unused-vars
async function _findAerodromePools(
  client: PublicClient,
  tokenAddress: string,
  config: (typeof CONFIG)[number],
): Promise<PoolInfo[]> {
  if (!config.aerodromeFactory) return [];

  const pools: PoolInfo[] = [];

  // Helper to check a pool
  const checkPool = async (
    baseTokenAddress: string,
    baseTokenSymbol: "USDC" | "WETH",
    stable: boolean,
  ) => {
    const poolAddress = await poolReadContract<Address>(client, {
      address: config.aerodromeFactory as Address,
      abi: aeroFactoryAbi as Abi,
      functionName: "getPool",
      args: [tokenAddress as Address, baseTokenAddress as Address, stable],
    });

    if (
      poolAddress &&
      poolAddress !== "0x0000000000000000000000000000000000000000"
    ) {
      const poolInfo = await getAeroPoolInfo(
        client,
        poolAddress,
        baseTokenSymbol,
        stable,
        config,
      );
      if (poolInfo) pools.push(poolInfo);
    }
  };

  // Check Stable and Volatile for both base tokens
  await Promise.all([
    checkPool(config.usdc, "USDC", false), // Volatile
    checkPool(config.usdc, "USDC", true), // Stable
    checkPool(config.weth, "WETH", false), // Volatile
    checkPool(config.weth, "WETH", true), // Stable
  ]);

  return pools;
}

/**
 * Find PancakeSwap V3 pools (Same interface as Uniswap V3) with retry and caching
 */
async function findPancakeswapPools(
  client: PublicClient,
  tokenAddress: string,
  config: (typeof CONFIG)[number],
): Promise<PoolInfo[]> {
  if (!config.pancakeswapFactory) return [];

  const pools: PoolInfo[] = [];

  // Helper to check a pool with retry
  const checkPool = async (
    baseTokenAddress: string,
    baseTokenSymbol: "USDC" | "WETH",
    fee: number,
  ) => {
    try {
      const poolAddress = await withRetryAndCache(
        `pancake-v3-pool:${config.pancakeswapFactory}:${tokenAddress}:${baseTokenAddress}:${fee}`,
        async () => {
          return poolReadContract<Address>(client, {
            address: config.pancakeswapFactory as Address,
            abi: uniFactoryAbi as Abi, // Compatible ABI
            functionName: "getPool",
            args: [tokenAddress as Address, baseTokenAddress as Address, fee],
          });
        },
        { cacheTtlMs: POOL_CACHE_TTL_MS },
      );

      if (
        poolAddress &&
        poolAddress !== "0x0000000000000000000000000000000000000000"
      ) {
        const poolInfo = await getUniPoolInfo(
          client,
          poolAddress,
          baseTokenSymbol,
          fee,
          config,
          tokenAddress,
        );
        if (poolInfo) {
          poolInfo.protocol = "Pancakeswap V3";
          pools.push(poolInfo);
        }
      }
    } catch {
      // Ignore pool lookup errors
    }
  };

  // Check all combinations
  // Only check USDC if NOT on BSC, because BSC USDC is 18 decimals and breaks SimplePoolOracle
  if (config.nativeToken !== "BNB") {
    await Promise.all([
      ...FEE_TIERS.map((fee) => checkPool(config.usdc, "USDC", fee)),
      ...FEE_TIERS.map((fee) => checkPool(config.weth, "WETH", fee)),
    ]);
  } else {
    // BSC: Only check WETH (WBNB)
    await Promise.all([
      ...FEE_TIERS.map((fee) => checkPool(config.weth, "WETH", fee)),
    ]);
  }

  return pools;
}

/**
 * Get detailed information about a Uniswap V3 pool
 */
async function getUniPoolInfo(
  client: PublicClient,
  poolAddress: string,
  baseToken: "USDC" | "WETH",
  fee: number,
  config: (typeof CONFIG)[number],
  targetTokenAddress: string,
): Promise<PoolInfo | null> {
  const [token0, token1, liquidity, slot0] = await Promise.all([
    poolReadContract<Address>(client, {
      address: poolAddress as Address,
      abi: uniPoolAbi as Abi,
      functionName: "token0",
    }),
    poolReadContract<Address>(client, {
      address: poolAddress as Address,
      abi: uniPoolAbi as Abi,
      functionName: "token1",
    }),
    poolReadContract<bigint>(client, {
      address: poolAddress as Address,
      abi: uniPoolAbi as Abi,
      functionName: "liquidity",
    }),
    poolReadContract<readonly [bigint, ...unknown[]]>(client, {
      address: poolAddress as Address,
      abi: uniPoolAbi as Abi,
      functionName: "slot0",
    }),
  ]);

  // Fetch decimals for price calculation
  const [decimals0, decimals1] = await Promise.all([
    poolReadContract<number>(client, {
      address: token0,
      abi: erc20Abi as Abi,
      functionName: "decimals",
    }),
    poolReadContract<number>(client, {
      address: token1,
      abi: erc20Abi as Abi,
      functionName: "decimals",
    }),
  ]);

  // Calculate real TVL using liquidity and sqrtPrice
  // slot0 returns a tuple: [sqrtPriceX96, tick, observationIndex, ...]
  const sqrtPriceX96 = slot0[0];
  const tvlUsd = calculateV3TVL(
    liquidity,
    sqrtPriceX96,
    token0,
    token1,
    baseToken,
    config,
  );

  // Calculate Price in USD
  // Price of Token0 in terms of Token1 = (sqrtPriceX96 / 2^96)^2
  // Adjusted Price = Price * 10^(decimals0 - decimals1)
  // We want price of TargetToken in USD.

  let priceUsd = 0;
  const isToken0Target =
    (token0 as string).toLowerCase() === targetTokenAddress.toLowerCase();

  // P = price of Token0 in Token1 (raw)
  const Q96 = 2n ** 96n;
  // Use number for price calc (precision loss acceptable for validation)
  const sqrtP = Number(sqrtPriceX96) / Number(Q96);
  const price0in1 = sqrtP * sqrtP;

  // Adjust for decimals: Price of 1 unit of Token0 = X units of Token1
  // Real Price 0 in 1 = price0in1 * 10^(decimals0 - decimals1) ??
  // Formula: price0 = (sqrtP^2) * (10^dec0 / 10^dec1) ??
  // Standard: price0 = price0in1 / (10^dec1 / 10^dec0) = price0in1 * 10^(dec0 - dec1)

  const decimalAdjustment = 10 ** (decimals0 - decimals1);
  const price0in1Adjusted = price0in1 * decimalAdjustment;

  // Base Token Price (USDC or ETH)
  let baseTokenPrice = 0;
  if (baseToken === "USDC") baseTokenPrice = 1;
  else baseTokenPrice = config.nativeTokenPriceEstimate; // e.g. 3200 for ETH

  if (isToken0Target) {
    // Target is Token0. We want price of Token0 in USD.
    // If Token1 is Base (Quote): Price0 = Price0in1 * PriceBase
    priceUsd = price0in1Adjusted * baseTokenPrice;
  } else {
    // Target is Token1. We want price of Token1 in USD.
    // Price1in0 = 1 / Price0in1
    // Price1 = (1 / price0in1Adjusted) * PriceBase
    priceUsd = (1 / price0in1Adjusted) * baseTokenPrice;
  }

  return {
    protocol: "Uniswap V3",
    address: poolAddress,
    token0: token0 as string,
    token1: token1 as string,
    fee,
    liquidity: BigInt(liquidity.toString()),
    tvlUsd,
    priceUsd,
    baseToken,
  };
}

/**
 * Get detailed information about an Aerodrome pool
 */
async function getAeroPoolInfo(
  client: PublicClient,
  poolAddress: string,
  baseToken: "USDC" | "WETH",
  stable: boolean,
  config: (typeof CONFIG)[number],
): Promise<PoolInfo | null> {
  const [token0, token1, reserve0, reserve1] = await Promise.all([
    poolReadContract<Address>(client, {
      address: poolAddress as Address,
      abi: aeroPoolAbi as Abi,
      functionName: "token0",
    }),
    poolReadContract<Address>(client, {
      address: poolAddress as Address,
      abi: aeroPoolAbi as Abi,
      functionName: "token1",
    }),
    poolReadContract<bigint>(client, {
      address: poolAddress as Address,
      abi: aeroPoolAbi as Abi,
      functionName: "reserve0",
    }),
    poolReadContract<bigint>(client, {
      address: poolAddress as Address,
      abi: aeroPoolAbi as Abi,
      functionName: "reserve1",
    }),
  ]);

  // Determine which reserve corresponds to the base token to estimate TVL
  // Note: This is a simplification. In production we should check decimals.
  const baseAddress = baseToken === "USDC" ? config.usdc : config.weth;
  const isToken0Base = token0.toLowerCase() === baseAddress.toLowerCase();
  const baseReserve = isToken0Base ? reserve0 : reserve1;

  // Estimate TVL: Base Reserve * 2 (assuming 50/50 pool value)
  // This works for both volatile and stable pools roughly
  const liquidity = baseReserve;

  // Normalize liquidity for the estimateTVL function which expects "units" roughly matching V3
  // But for V2 style, we can just calculate USD value directly
  let tvlUsd = 0;
  if (baseToken === "USDC") {
    tvlUsd = (Number(liquidity) / 1e6) * 2;
  } else {
    tvlUsd = (Number(liquidity) / 1e18) * 3000 * 2;
  }

  return {
    protocol: "Aerodrome",
    address: poolAddress,
    token0: token0 as string,
    token1: token1 as string,
    stable,
    liquidity, // This is raw reserve, different from V3 liquidity
    tvlUsd,
    baseToken,
  };
}

/**
 * Calculate V3 TVL in USD
 */
function calculateV3TVL(
  liquidity: bigint,
  sqrtPriceX96: bigint,
  token0: string,
  token1: string,
  baseTokenSymbol: "USDC" | "WETH",
  config: (typeof CONFIG)[number],
): number {
  if (liquidity === 0n) return 0;

  const Q96 = 2n ** 96n;
  const baseAddress = baseTokenSymbol === "USDC" ? config.usdc : config.weth;
  const isToken0Base = token0.toLowerCase() === baseAddress.toLowerCase();

  // Calculate amount of base token in the pool
  // L = sqrt(x * y)
  // sqrtP = y / x (price of 0 in terms of 1? No, price of 0 in terms of 1 is y/x if 0 is base... wait)
  // Uniswap Price P is amount of token1 per 1 token0.
  // P = y / x. sqrtP = sqrt(y/x).
  // sqrtPriceX96 = sqrt(P) * 2^96.

  // If Token0 is Base (x is Base):
  // We want x.
  // x = L / sqrtP = L * 2^96 / sqrtPriceX96.

  // If Token1 is Base (y is Base):
  // We want y.
  // y = L * sqrtP = L * sqrtPriceX96 / 2^96.

  let baseAmount = 0n;

  if (isToken0Base) {
    // Base is x
    baseAmount = (liquidity * Q96) / sqrtPriceX96;
  } else {
    // Base is y
    baseAmount = (liquidity * sqrtPriceX96) / Q96;
  }

  // Convert to USD
  // TVL is roughly 2x the base token amount (50/50 ratio in V3 at current tick range approx)
  // Actually V3 is concentrated, but for ranking, assuming 2x base is fair enough vs Aerodrome.

  if (baseTokenSymbol === "USDC") {
    // 6 decimals
    return (Number(baseAmount) / 1e6) * 2;
  } else {
    // 18 decimals (WETH/WBNB)
    return (Number(baseAmount) / 1e18) * config.nativeTokenPriceEstimate * 2;
  }
}

/**
 * Validate pool has sufficient liquidity
 */
export function validatePoolLiquidity(pool: PoolInfo): {
  valid: boolean;
  warning?: string;
} {
  const MIN_LIQUIDITY_USD = 10000; // $10k minimum (lowered to be more permissive for finding)

  if (pool.tvlUsd < MIN_LIQUIDITY_USD) {
    return {
      valid: false,
      warning: `Low liquidity: $${pool.tvlUsd.toLocaleString()}. Minimum recommended: $${MIN_LIQUIDITY_USD.toLocaleString()}`,
    };
  }

  return { valid: true };
}

/**
 * Format pool info for display
 */
export function formatPoolInfo(pool: PoolInfo): string {
  if (pool.protocol === "Aerodrome") {
    const type = pool.stable ? "Stable" : "Volatile";
    return `Aerodrome ${type} (${pool.baseToken}) - TVL: ~$${Math.floor(pool.tvlUsd).toLocaleString()}`;
  }
  const feePercent = ((pool.fee || 0) / 10000).toFixed(2);
  return `${pool.protocol} (${feePercent}%, ${pool.baseToken}) - TVL: ~$${Math.floor(pool.tvlUsd).toLocaleString()}`;
}
