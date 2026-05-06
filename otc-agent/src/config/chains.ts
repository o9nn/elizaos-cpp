import {
  base,
  baseSepolia,
  bsc,
  bscTestnet,
  localhost,
  type Chain as ViemChain,
} from "viem/chains";
import { getContracts, getCurrentNetwork } from "./contracts";

// String-based chain identifier for database/API (lowercase, URL-safe)
export type Chain = "ethereum" | "base" | "bsc" | "solana";
export type ChainFamily = "evm" | "solana";

export interface ChainConfig {
  id: string; // String ID for database storage
  name: string;
  rpcUrl: string;
  explorerUrl: string;
  nativeCurrency: {
    name: string;
    symbol: string;
    decimals: number;
  };
  contracts: {
    otc?: string;
    usdc?: string;
  };
  type: ChainFamily;
  viemChain?: ViemChain; // Reference to viem chain for wagmi (EVM only)
  chainId?: number; // Numeric chain ID (EVM only)
}

// Use centralized network resolution from contracts.ts
const env = getCurrentNetwork();
const deployments = getContracts(env);

export const SUPPORTED_CHAINS: Record<Chain, ChainConfig> = {
  ethereum: {
    id: localhost.id.toString(),
    name: "Anvil Local",
    rpcUrl: process.env.NEXT_PUBLIC_RPC_URL || "http://127.0.0.1:8545",
    explorerUrl: "http://localhost:8545",
    nativeCurrency: { name: "Ether", symbol: "ETH", decimals: 18 },
    contracts: {
      otc:
        deployments.evm?.contracts?.otc || process.env.NEXT_PUBLIC_OTC_ADDRESS,
      usdc:
        deployments.evm?.contracts?.usdc ||
        process.env.NEXT_PUBLIC_USDC_ADDRESS,
    },
    type: "evm",
    viemChain: localhost,
    chainId: localhost.id,
  },
  base: (() => {
    const isMainnet = env === "mainnet";
    const chain = isMainnet ? base : baseSepolia;
    
    // Hardcoded mainnet/testnet OTC addresses from deployment configs
    const MAINNET_OTC = "0x12FA61c9d77AEd9BeDA0FF4bF2E900F31bdBdc45";
    const TESTNET_OTC = "0x08cAa161780d195E0799b73b318da5D175b85313";
    
    // Use deployments if available, else env vars, else hardcoded defaults
    const otc =
      deployments.evm?.contracts?.otc ||
      process.env.NEXT_PUBLIC_BASE_OTC_ADDRESS ||
      (isMainnet ? MAINNET_OTC : TESTNET_OTC);

    // For mainnet, use proxy route to keep Alchemy key server-side
    // For testnet, use public Sepolia RPC
    const rpcUrl =
      process.env.NEXT_PUBLIC_BASE_RPC_URL ||
      (isMainnet ? "/api/rpc/base" : "https://sepolia.base.org");

    return {
      id: chain.id.toString(),
      name: isMainnet ? "Base" : "Base Sepolia",
      rpcUrl,
      explorerUrl: isMainnet
        ? "https://basescan.org"
        : "https://sepolia.basescan.org",
      nativeCurrency: { name: "Ether", symbol: "ETH", decimals: 18 },
      contracts: {
        otc: otc,
        usdc: isMainnet
          ? "0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913"
          : "0x036CbD53842c5426634e7929541eC2318f3dCF7e",
      },
      type: "evm" as ChainFamily,
      viemChain: chain,
      chainId: chain.id,
    };
  })(),
  bsc: (() => {
    const isMainnet = env === "mainnet";
    const chain = isMainnet ? bsc : bscTestnet;
    const otc =
      deployments.evm?.contracts?.otc ||
      process.env.NEXT_PUBLIC_BSC_OTC_ADDRESS;

    return {
      id: chain.id.toString(),
      name: isMainnet ? "BSC" : "BSC Testnet",
      rpcUrl:
        process.env.NEXT_PUBLIC_BSC_RPC_URL ||
        (isMainnet
          ? "https://bsc-dataseed1.binance.org"
          : "https://data-seed-prebsc-1-s1.binance.org:8545"),
      explorerUrl: isMainnet
        ? "https://bscscan.com"
        : "https://testnet.bscscan.com",
      nativeCurrency: { name: "BNB", symbol: "BNB", decimals: 18 },
      contracts: {
        otc: otc,
        usdc: "0x8AC76a51cc950d9822D68b83fE1Ad97B32Cd580d",
      },
      type: "evm" as ChainFamily,
      viemChain: chain,
      chainId: chain.id,
    };
  })(),
  solana: (() => {
    const isMainnet = env === "mainnet";
    const isLocal = env === "local";

    const otc =
      deployments.solana?.NEXT_PUBLIC_SOLANA_DESK ||
      process.env.NEXT_PUBLIC_SOLANA_DESK;
    const usdc =
      deployments.solana?.NEXT_PUBLIC_SOLANA_USDC_MINT ||
      process.env.NEXT_PUBLIC_SOLANA_USDC_MINT;

    return {
      id: isMainnet
        ? "solana-mainnet"
        : isLocal
          ? "solana-localnet"
          : "solana-devnet",
      name: isMainnet
        ? "Solana"
        : isLocal
          ? "Solana Localnet"
          : "Solana Devnet",
      rpcUrl:
        process.env.NEXT_PUBLIC_SOLANA_RPC ||
        (isMainnet
          ? "https://api.mainnet-beta.solana.com"
          : isLocal
            ? "http://127.0.0.1:8899"
            : "https://api.devnet.solana.com"),
      explorerUrl: "https://explorer.solana.com",
      nativeCurrency: { name: "SOL", symbol: "SOL", decimals: 9 },
      contracts: {
        otc: otc,
        usdc: isMainnet
          ? "EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"
          : usdc || "Gh9ZwEmdLJ8DscKNTkTqPbNwLNNBjuSzaG9Vp2KGtKJr", // Default to Devnet USDC if not in deployment
      },
      type: "solana" as ChainFamily,
    };
  })(),
};

/**
 * Get chain config by identifier
 */
export function getChainConfig(chain: Chain): ChainConfig {
  return SUPPORTED_CHAINS[chain];
}

/**
 * Check if chain is EVM-based
 */
export function isEVMChain(chain: Chain): boolean {
  return SUPPORTED_CHAINS[chain].type === "evm";
}

/**
 * Check if chain is Solana-based
 */
export function isSolanaChain(chain: Chain): boolean {
  return SUPPORTED_CHAINS[chain].type === "solana";
}

/**
 * Get chain identifier from string chain ID (database format)
 */
export function getChainFromId(chainId: string): Chain | null {
  for (const [key, config] of Object.entries(SUPPORTED_CHAINS)) {
    if (config.id === chainId) return key as Chain;
  }
  return null;
}

/**
 * Get chain identifier from numeric chain ID (wagmi/viem format)
 */
export function getChainFromNumericId(chainId: number): Chain | null {
  for (const [key, config] of Object.entries(SUPPORTED_CHAINS)) {
    if (config.chainId === chainId) return key as Chain;
  }
  return null;
}

/**
 * Get all viem chains for wagmi configuration
 */
export function getAllViemChains(): ViemChain[] {
  return Object.values(SUPPORTED_CHAINS)
    .filter((config) => config.viemChain)
    .map((config) => config.viemChain!);
}
