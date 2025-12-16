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



/**
 * Supported blockchain networks
 */
using SupportedNetwork = std::variant<'base', 'ethereum', 'polygon', 'arbitrum', 'optimism', 'scroll', 'base-sepolia', 'ethereum-sepolia'>;

/**
 * Chain configuration interface
 */
struct ChainConfig {
    std::string name;
    Chain chain;
    (alchemyKey: string) => string rpcUrl;
    std::string explorerUrl;
    { nativeToken;
    std::string symbol;
    std::string name;
    std::string coingeckoId;
    double decimals;
    std::string coingeckoPlatform;
    { swap;
    boolean; // Does CDP SDK support swaps on this network? cdpSupported;
};

/**
 * Centralized chain configurations
 * Add new chains here to support them across the entire application
 */
const CHAIN_CONFIGS: Record<SupportedNetwork, ChainConfig> = {
  'base': {
    name: 'Base',
    chain: base,
    rpcUrl: (alchemyKey: string) => `https://base-mainnet.g.alchemy.com/v2/${alchemyKey}`,
    explorerUrl: 'https://basescan.org',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      coingeckoId: 'ethereum',
      decimals: 18,
    },
    coingeckoPlatform: 'base',
    swap: {
      cdpSupported: true, // CDP SDK supports Base swaps
    },
  },
  'ethereum': {
    name: 'Ethereum',
    chain: mainnet,
    rpcUrl: (alchemyKey: string) => `https://eth-mainnet.g.alchemy.com/v2/${alchemyKey}`,
    explorerUrl: 'https://etherscan.io',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      coingeckoId: 'ethereum',
      decimals: 18,
    },
    coingeckoPlatform: 'ethereum',
    swap: {
      cdpSupported: true, // CDP SDK supports Ethereum swaps
    },
  },
  'polygon': {
    name: 'Polygon',
    chain: polygon,
    rpcUrl: (alchemyKey: string) => `https://polygon-mainnet.g.alchemy.com/v2/${alchemyKey}`,
    explorerUrl: 'https://polygonscan.com',
    nativeToken: {
      symbol: 'POL',
      name: 'Polygon',
      coingeckoId: 'polygon-ecosystem-token',
      decimals: 18,
    },
    coingeckoPlatform: 'polygon-pos',
    swap: {
      cdpSupported: false, // CDP SDK does NOT support Polygon swaps
    },
  },
  'arbitrum': {
    name: 'Arbitrum',
    chain: arbitrum,
    rpcUrl: (alchemyKey: string) => `https://arb-mainnet.g.alchemy.com/v2/${alchemyKey}`,
    explorerUrl: 'https://arbiscan.io',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      coingeckoId: 'ethereum',
      decimals: 18,
    },
    coingeckoPlatform: 'arbitrum-one',
    swap: {
      cdpSupported: false, // CDP SDK does NOT support Arbitrum swaps
    },
  },
  'optimism': {
    name: 'Optimism',
    chain: optimism,
    rpcUrl: (alchemyKey: string) => `https://opt-mainnet.g.alchemy.com/v2/${alchemyKey}`,
    explorerUrl: 'https://optimistic.etherscan.io',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      coingeckoId: 'ethereum',
      decimals: 18,
    },
    coingeckoPlatform: 'optimistic-ethereum',
    swap: {
      cdpSupported: false, // CDP SDK does NOT support Optimism swaps
    },
  },
  'scroll': {
    name: 'Scroll',
    chain: scroll,
    rpcUrl: (alchemyKey: string) => `https://scroll-mainnet.g.alchemy.com/v2/${alchemyKey}`,
    explorerUrl: 'https://scrollscan.com',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      coingeckoId: 'ethereum',
      decimals: 18,
    },
    coingeckoPlatform: 'scroll',
    swap: {
      cdpSupported: false, // CDP SDK does NOT support Scroll swaps
    },
  },
  'base-sepolia': {
    name: 'Base Sepolia',
    chain: baseSepolia,
    rpcUrl: (alchemyKey: string) => `https://base-sepolia.g.alchemy.com/v2/${alchemyKey}`,
    explorerUrl: 'https://sepolia.basescan.org',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      coingeckoId: 'ethereum',
      decimals: 18,
    },
    coingeckoPlatform: 'base',
    swap: {
      cdpSupported: false, // Testnet - no CDP swap support
    },
  },
  'ethereum-sepolia': {
    name: 'Ethereum Sepolia',
    chain: sepolia,
    rpcUrl: (alchemyKey: string) => `https://eth-sepolia.g.alchemy.com/v2/${alchemyKey}`,
    explorerUrl: 'https://sepolia.etherscan.io',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      coingeckoId: 'ethereum',
      decimals: 18,
    },
    coingeckoPlatform: 'ethereum',
    swap: {
      cdpSupported: false, // Testnet - no CDP swap support
    },
  },
};

/**
 * Get mainnet networks only (excludes testnets)
 */
const MAINNET_NETWORKS: SupportedNetwork[] = ['base', 'ethereum', 'polygon', 'arbitrum', 'optimism', 'scroll'];

/**
 * Get testnet networks only
 */
const TESTNET_NETWORKS: SupportedNetwork[] = ['base-sepolia', 'ethereum-sepolia'];

/**
 * Get all supported networks
 */
const ALL_NETWORKS: SupportedNetwork[] = Object.keys(CHAIN_CONFIGS) as SupportedNetwork[];

/**
 * Helper: Get chain config by network name
 */
ChainConfig | null getChainConfig(const std::string& network);

/**
 * Helper: Get viem chain object by network name
 */
Chain | null getViemChain(const std::string& network);

/**
 * Helper: Get RPC URL for a network
 */
string | null getRpcUrl(const std::string& network, const std::string& alchemyKey);

/**
 * Helper: Get explorer URL for a network
 */
string | null getExplorerUrl(const std::string& network);

/**
 * Helper: Get transaction explorer URL
 */
string | null getTxExplorerUrl(const std::string& network, const std::string& txHash);

/**
 * Helper: Get address explorer URL
 */
string | null getAddressExplorerUrl(const std::string& network, const std::string& address);

/**
 * Helper: Get native token info for a network
 */
void getNativeTokenInfo(const std::string& network);

/**
 * Helper: Get CoinGecko platform ID for a network
 */
string | null getCoingeckoPlatform(const std::string& network);

/**
 * Helper: Check if a network is supported
 */
network is SupportedNetwork isSupportedNetwork(const std::string& network);

/**
 * Helper: Check if a network is a mainnet
 */
bool isMainnet(const std::string& network);

/**
 * Helper: Check if a network is a testnet
 */
bool isTestnet(const std::string& network);

/**
 * Helper: Check if CDP SDK supports swaps on a network
 */
bool isCdpSwapSupported(const std::string& network);

/**
 * Helper: Get networks that support CDP swaps
 */
std::vector<SupportedNetwork> getCdpSwapSupportedNetworks();

// ============================================================================
// Swap Protocol Constants
// ============================================================================

/**
 * Native token address used by swap protocols (0x + Ee repeated)
 * This special address represents native tokens (ETH, MATIC, etc.) in swap protocols
 */
const NATIVE_TOKEN_ADDRESS = '0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE';

/**
 * Normalize token address for swap protocols
 * If the token address is not a valid contract address (0x...), treat it as native token
 */
std::string normalizeTokenAddress(const std::string& token);

/**
 * Uniswap V3 SwapRouter addresses per network
 */
const UNISWAP_V3_ROUTER: Record<string, string> = {
  'ethereum': '0xE592427A0AEce92De3Edee1F18E0157C05861564',
  'polygon': '0xE592427A0AEce92De3Edee1F18E0157C05861564',
  'arbitrum': '0xE592427A0AEce92De3Edee1F18E0157C05861564',
  'optimism': '0xE592427A0AEce92De3Edee1F18E0157C05861564',
  'base': '0x2626664c2603336E57B271c5C0b26F421741e481',
};

/**
 * Uniswap V3 Quoter V2 addresses per network
 */
const UNISWAP_V3_QUOTER: Record<string, string> = {
  'ethereum': '0x61fFE014bA17989E743c5F6cB21bF9697530B21e',
  'polygon': '0x61fFE014bA17989E743c5F6cB21bF9697530B21e',
  'arbitrum': '0x61fFE014bA17989E743c5F6cB21bF9697530B21e',
  'optimism': '0x61fFE014bA17989E743c5F6cB21bF9697530B21e',
  'base': '0x3d4e44Eb1374240CE5F1B871ab261CD16335B76a',
};

/**
 * Wrapped native token addresses per network
 * Uniswap V3 requires wrapped tokens for native currency swaps
 */
const WRAPPED_NATIVE_TOKEN: Record<string, string> = {
  'ethereum': '0xC02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2', // WETH
  'polygon': '0x0d500B1d8E8eF31E21C99d1Db9A6444d3ADf1270',  // WMATIC
  'arbitrum': '0x82aF49447D8a07e3bd95BD0d56f35241523fBab1', // WETH
  'optimism': '0x4200000000000000000000000000000000000006', // WETH
  'base': '0x4200000000000000000000000000000000000006',     // WETH
};

/**
 * Uniswap V3 pool fee tiers (in hundredths of a bip, i.e. 1e-6)
 */
const UNISWAP_POOL_FEES = {
  LOW: 500,      // 0.05%
  MEDIUM: 3000,  // 0.3%
  HIGH: 10000,   // 1%
};

} // namespace elizaos
