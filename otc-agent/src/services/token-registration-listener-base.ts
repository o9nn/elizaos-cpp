import { createPublicClient, http, parseAbi } from "viem";
import { base } from "viem/chains";
import { TokenRegistryService } from "./tokenRegistry";

const ERC20_ABI = parseAbi([
  "function symbol() view returns (string)",
  "function name() view returns (string)",
  "function decimals() view returns (uint8)",
]);

let isListening = false;

/**
 * Start listening for TokenRegistered events from RegistrationHelper
 */
export async function startBaseListener() {
  if (isListening) {
    console.warn("[Base Listener] Already listening");
    return;
  }

  const registrationHelperAddress =
    process.env.NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS;
  if (!registrationHelperAddress) {
    console.error("[Base Listener] REGISTRATION_HELPER_ADDRESS not configured");
    return;
  }

  const rpcUrl =
    process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://mainnet.base.org";

  const client = createPublicClient({
    chain: base,
    transport: http(rpcUrl),
  });

  console.log(
    "[Base Listener] Starting listener for",
    registrationHelperAddress,
  );
  isListening = true;

  // Watch for TokenRegistered events
  const unwatch = client.watchEvent({
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
    onLogs: async (logs) => {
      for (const log of logs) {
        await handleTokenRegistered(client, log);
      }
    },
    onError: (error) => {
      console.error("[Base Listener] Error:", error);
    },
  });

  // Handle graceful shutdown
  process.on("SIGINT", () => {
    console.log("[Base Listener] Stopping...");
    unwatch();
    isListening = false;
  });

  process.on("SIGTERM", () => {
    console.log("[Base Listener] Stopping...");
    unwatch();
    isListening = false;
  });

  console.log("[Base Listener] Now listening for token registrations");
}

/**
 * Handle a TokenRegistered event
 */
async function handleTokenRegistered(client: any, log: any) {
  // When using watchEvent with event definition, viem automatically decodes the log
  const { tokenAddress, registeredBy } = log.args as {
    tokenId: string;
    tokenAddress: string;
    pool: string;
    registeredBy: string;
  };

  console.log(
    "[Base Listener] Token registered:",
    tokenAddress,
    "by",
    registeredBy,
  );

  // Fetch token metadata from blockchain
  const [symbol, name, decimals] = await Promise.all([
    client.readContract({
      address: tokenAddress as `0x${string}`,
      abi: ERC20_ABI,
      functionName: "symbol",
    }),
    client.readContract({
      address: tokenAddress as `0x${string}`,
      abi: ERC20_ABI,
      functionName: "name",
    }),
    client.readContract({
      address: tokenAddress as `0x${string}`,
      abi: ERC20_ABI,
      functionName: "decimals",
    }),
  ]);

  // Add to database
  const tokenService = new TokenRegistryService();
  await tokenService.registerToken({
    symbol: symbol as string,
    name: name as string,
    contractAddress: tokenAddress.toLowerCase(),
    chain: "base",
    decimals: Number(decimals),
    logoUrl: undefined, // Could fetch from a token list
    description: `Registered via RegistrationHelper by ${registeredBy}`,
  });

  console.log(
    `[Base Listener] ✅ Successfully registered ${symbol} (${tokenAddress}) to database`,
  );
}

/**
 * Backfill historical events (run once after deployment)
 */
export async function backfillBaseEvents(fromBlock?: bigint) {
  const registrationHelperAddress =
    process.env.NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS;
  if (!registrationHelperAddress) {
    throw new Error("REGISTRATION_HELPER_ADDRESS not configured");
  }

  const rpcUrl =
    process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://mainnet.base.org";

  const client = createPublicClient({
    chain: base,
    transport: http(rpcUrl),
  });

  const latestBlock = await client.getBlockNumber();
  const startBlock = fromBlock || latestBlock - BigInt(10000); // Last ~10k blocks

  console.log(
    `[Base Backfill] Fetching events from block ${startBlock} to ${latestBlock}`,
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

  console.log(`[Base Backfill] Found ${logs.length} TokenRegistered events`);

  for (const log of logs) {
    await handleTokenRegistered(client, log);
  }

  console.log("[Base Backfill] ✅ Backfill complete");
}
