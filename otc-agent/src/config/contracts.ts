import localEvm from "./deployments/local-evm.json";
import localSolana from "./deployments/local-solana.json";
import testnetEvm from "./deployments/testnet-evm.json";
import testnetSolana from "./deployments/testnet-solana.json";
import mainnetEvm from "./deployments/mainnet-evm.json";
import mainnetSolana from "./deployments/mainnet-solana.json";

// =============================================================================
// TYPES
// =============================================================================

export interface EvmDeployment {
  network: string;
  chainId: number;
  rpc: string;
  deployer?: string;
  contracts: {
    otc: string;
    usdc: string;
    elizaToken?: string;
    registrationHelper?: string;
    elizaUsdFeed?: string;
    ethUsdFeed?: string;
  };
  accounts?: {
    owner?: string;
    agent?: string;
    approver?: string;
    testWallet?: string;
  };
}

export interface SolanaDeployment {
  network: string;
  rpc: string;
  deployer?: string;
  programId: string;
  desk: string;
  deskOwner?: string;
  usdcMint: string;
  registeredTokens?: Record<string, {
    mint: string;
    registry: string;
    treasury: string;
    priceUsd?: number;
  }>;
}

// =============================================================================
// DEPLOYMENT CONFIGS
// =============================================================================

export const CONTRACT_DEPLOYMENTS = {
  local: {
    evm: localEvm as EvmDeployment,
    solana: localSolana as SolanaDeployment,
  },
  testnet: {
    evm: testnetEvm as EvmDeployment,
    solana: testnetSolana as SolanaDeployment,
  },
  mainnet: {
    evm: mainnetEvm as EvmDeployment,
    solana: mainnetSolana as SolanaDeployment,
  },
};

export type NetworkType = "local" | "testnet" | "mainnet";

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/**
 * Get the current network from env or default to mainnet
 * Priority: NEXT_PUBLIC_NETWORK > NEXT_PUBLIC_USE_MAINNET > default to mainnet
 * 
 * This is the SINGLE SOURCE OF TRUTH for network resolution.
 * Do not duplicate this logic elsewhere.
 */
export function getCurrentNetwork(): NetworkType {
  const explicitNetwork = process.env.NEXT_PUBLIC_NETWORK || process.env.NETWORK;
  
  if (explicitNetwork === "mainnet") return "mainnet";
  if (explicitNetwork === "testnet" || explicitNetwork === "sepolia") return "testnet";
  if (explicitNetwork === "local" || explicitNetwork === "localnet" || explicitNetwork === "anvil") return "local";
  
  // Legacy flag support
  if (process.env.NEXT_PUBLIC_USE_MAINNET === "true") return "mainnet";
  
  // Default to mainnet for production
  return "mainnet";
}

/**
 * Get deployment configs for a network
 */
export function getContracts(network?: NetworkType) {
  const net = network || getCurrentNetwork();
  return CONTRACT_DEPLOYMENTS[net];
}

/**
 * Get EVM contract addresses with env overrides
 */
export function getEvmConfig(network?: NetworkType): EvmDeployment {
  const net = network || getCurrentNetwork();
  const config = CONTRACT_DEPLOYMENTS[net].evm;
  
  // Allow env overrides
  return {
    ...config,
    rpc: process.env.NEXT_PUBLIC_RPC_URL || config.rpc,
    contracts: {
      ...config.contracts,
      otc: process.env.NEXT_PUBLIC_OTC_ADDRESS || 
           (net === "mainnet" ? process.env.NEXT_PUBLIC_OTC_ADDRESS_MAINNET : null) || 
           config.contracts.otc,
      usdc: process.env.NEXT_PUBLIC_USDC_ADDRESS || config.contracts.usdc,
    },
    accounts: {
      ...config.accounts,
      approver: process.env.APPROVER_ADDRESS || config.accounts?.approver,
    },
  };
}

/**
 * Get Solana config with env overrides
 */
export function getSolanaConfig(network?: NetworkType): SolanaDeployment {
  const net = network || getCurrentNetwork();
  const config = CONTRACT_DEPLOYMENTS[net].solana;
  
  // Allow env overrides
  return {
    ...config,
    rpc: process.env.NEXT_PUBLIC_SOLANA_RPC || 
         (net === "mainnet" ? process.env.NEXT_PUBLIC_SOLANA_RPC_MAINNET : null) || 
         config.rpc,
    programId: process.env.NEXT_PUBLIC_SOLANA_PROGRAM_ID || config.programId,
    desk: process.env.NEXT_PUBLIC_SOLANA_DESK || 
          (net === "mainnet" ? process.env.NEXT_PUBLIC_SOLANA_DESK_MAINNET : null) || 
          config.desk,
    usdcMint: process.env.NEXT_PUBLIC_SOLANA_USDC_MINT || config.usdcMint,
  };
}

/**
 * Get OTC contract address for current network
 */
export function getOtcAddress(network?: NetworkType): string {
  const config = getEvmConfig(network);
  return config.contracts.otc;
}

/**
 * Get Solana desk address for current network
 */
export function getSolanaDesk(network?: NetworkType): string {
  const config = getSolanaConfig(network);
  return config.desk;
}

/**
 * Get Solana program ID for current network
 */
export function getSolanaProgramId(network?: NetworkType): string {
  const config = getSolanaConfig(network);
  return config.programId;
}
