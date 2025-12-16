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
 * Frontend Chain Configuration
 * 
 * Centralized configuration for blockchain networks in the UI
 */

/**
 * Supported blockchain networks
 */
using SupportedChain = std::variant<'base', 'ethereum', 'polygon', 'arbitrum', 'optimism', 'scroll'>;

/**
 * Chain UI configuration interface
 */
struct ChainUIConfig {
    SupportedChain id;
    std::string name;
    std::string displayName;
    string; // Path to icon/logo icon;
    string; // Path to wallet icon walletIcon;
    { nativeToken;
    std::string symbol;
    std::string name;
    string; // Path to token icon icon;
    std::string explorerUrl;
    string; // Brand color for the chain color;
};


/**
 * Centralized chain UI configurations
 */
const CHAIN_UI_CONFIGS: Record<SupportedChain, ChainUIConfig> = {
  base: {
    id: 'base',
    name: 'Base',
    displayName: 'Base',
    icon: '/assets/base.svg',
    walletIcon: '/assets/walletIcon/base.svg',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      icon: '/assets/eth.svg',
    },
    explorerUrl: 'https://basescan.org',
    color: '#0052FF', // Base blue
  },
  ethereum: {
    id: 'ethereum',
    name: 'Ethereum',
    displayName: 'Ethereum',
    icon: '/assets/eth.svg',
    walletIcon: '/assets/walletIcon/ethereum.svg',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      icon: '/assets/eth.svg',
    },
    explorerUrl: 'https://etherscan.io',
    color: '#627EEA', // Ethereum purple
  },
  polygon: {
    id: 'polygon',
    name: 'Polygon',
    displayName: 'Polygon',
    icon: '/assets/polygon.svg',
    walletIcon: '/assets/walletIcon/polygon.svg',
    nativeToken: {
      symbol: 'POL',
      name: 'Polygon',
      icon: '/assets/polygon.svg',
    },
    explorerUrl: 'https://polygonscan.com',
    color: '#8247E5', // Polygon purple
  },
  arbitrum: {
    id: 'arbitrum',
    name: 'Arbitrum',
    displayName: 'Arbitrum',
    icon: '/assets/arbitrum.svg',
    walletIcon: '/assets/walletIcon/arbitrum.svg',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      icon: '/assets/eth.svg',
    },
    explorerUrl: 'https://arbiscan.io',
    color: '#28A0F0', // Arbitrum blue
  },
  optimism: {
    id: 'optimism',
    name: 'Optimism',
    displayName: 'Optimism',
    icon: '/assets/optimism.svg',
    walletIcon: '/assets/walletIcon/optimism.svg',
      nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      icon: '/assets/eth.svg',
    },
    explorerUrl: 'https://optimistic.etherscan.io',
    color: '#FF0420', // Optimism red
  },
  scroll: {
    id: 'scroll',
    name: 'Scroll',
    displayName: 'Scroll',
    icon: '/assets/scroll.svg',
    walletIcon: '/assets/walletIcon/scroll.svg',
    nativeToken: {
      symbol: 'ETH',
      name: 'Ethereum',
      icon: '/assets/eth.svg',
    },
    explorerUrl: 'https://scrollscan.com',
    color: '#FFEEDA', // Scroll beige
  },
};

/**
 * All supported chains as an array
 */
const SUPPORTED_CHAINS: SupportedChain[] = Object.keys(CHAIN_UI_CONFIGS) as SupportedChain[];

/**
 * Helper: Get chain config by chain name
 */


/**
 * Helper: Get chain icon path
 */


/**
 * Helper: Get chain wallet icon path
 */

/**
 * Helper: Get native token icon path by chain
 */


/**
 * Helper: Get native token symbol by chain
 */


/**
 * Helper: Get chain color
 */


/**
 * Helper: Get transaction explorer URL
 */
/tx/${txHash}` : null;
}

/**
 * Helper: Get address explorer URL
 */
/address/${address}` : null;
}

/**
 * Helper: Check if a chain is supported
 */


/**
 * Helper: Get chain display name
 */


/**
 * Token icon mapping for common tokens
 * Maps token symbol to icon path
 */
const TOKEN_ICONS: Record<string, string> = {
  ETH: '/assets/eth.svg',
  WETH: '/assets/eth.svg',
  MATIC: '/assets/polygon.svg',
  POL: '/assets/polygon.svg',
  // Add more common tokens as needed
  USDC: '/assets/usdc.svg',
  USDT: '/assets/usdt.svg',
  DAI: '/assets/dai.svg',
};

/**
 * Helper: Get token icon by symbol
 * Returns null if no icon is available (will use fallback in component)
 */


/**
 * Helper: Get token icon by symbol or chain
 * First tries to get from TOKEN_ICONS, then falls back to native token icon
 */


  return null;
}


} // namespace elizaos
