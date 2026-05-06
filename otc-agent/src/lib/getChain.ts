import {
  base,
  baseSepolia,
  bsc,
  bscTestnet,
  localhost,
  type Chain,
} from "viem/chains";
import { getCurrentNetwork, getEvmConfig } from "@/config/contracts";

// Anvil chain with correct chain ID (31337)
const anvil: Chain = {
  ...localhost,
  id: 31337,
  name: "Anvil",
};

/**
 * Get the appropriate chain based on environment and configuration
 * Supports: Base, BSC, Anvil/localhost
 */
export function getChain(): Chain {
  const network = getCurrentNetwork();

  // Handle unified network names
  if (network === "mainnet") return base;
  if (network === "testnet") return baseSepolia;
  if (network === "local") return anvil;

  // Default to Base mainnet
  return base;
}

/**
 * Get RPC URL for the current chain
 * Uses deployment config with env override support
 */
export function getRpcUrl(): string {
  const config = getEvmConfig();
  return config.rpc;
}

/**
 * Get RPC URL for a specific chain type
 * @param chainType - Chain identifier (base, bsc, localhost, etc.)
 */
export function getRpcUrlForChain(chainType: string): string {
  switch (chainType) {
    case "base":
      return process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://mainnet.base.org";
    case "base-sepolia":
      return process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://sepolia.base.org";
    case "bsc":
      return (
        process.env.NEXT_PUBLIC_BSC_RPC_URL ||
        "https://bsc-dataseed1.binance.org"
      );
    case "bsc-testnet":
      return (
        process.env.NEXT_PUBLIC_BSC_RPC_URL ||
        "https://data-seed-prebsc-1-s1.binance.org:8545"
      );
    case "localhost":
    case "anvil":
      return process.env.NEXT_PUBLIC_RPC_URL || "http://127.0.0.1:8545";
    default:
      return getRpcUrl();
  }
}
