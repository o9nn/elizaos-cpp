#include "actions/cdp-check-tx-confirmation.hpp"
#include "actions/cdp-resolve-ens.hpp"
#include "actions/cdp-tx-explorer-link.hpp"
#include "actions/cdp-wallet-balance.hpp"
#include "actions/cdp-wallet-check-balance.hpp"
#include "actions/cdp-wallet-create.hpp"
#include "actions/cdp-wallet-fetch-with-payment.hpp"
#include "actions/cdp-wallet-info.hpp"
#include "actions/cdp-wallet-nft-transfer.hpp"
#include "actions/cdp-wallet-swap.hpp"
#include "actions/cdp-wallet-token-transfer.hpp"
#include "actions/cdp-wallet-unwrap.hpp"
#include "providers/walletState.hpp"
#include "services/cdp.service.hpp"
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

import type { Plugin } from "@elizaos/core";

// Services
;

// Actions
// ;
// ;
;
;
;
;
;
;
;
;
// ;
// ;

// Providers
;

// Types
type { CdpNetwork } from "./types";

/**
 * CDP Plugin
 * 
 * Provides Coinbase Developer Platform integration for:
 * - Wallet management (balances, tokens, NFTs)
 * - Token transfers and swaps
 * - NFT transfers
 * - x402 paid API requests (new!)
 * 
 * Actions:
 * - USER_WALLET_INFO: View wallet balances and assets
 * - CHECK_TOKEN_BALANCE: Fast balance check for specific token (optimized for transaction validation)
 * - USER_WALLET_TOKEN_TRANSFER: Transfer ERC20 tokens
 * - USER_WALLET_NFT_TRANSFER: Transfer NFTs
 * - USER_WALLET_SWAP: Swap tokens via DEX aggregators
 * - GET_TX_EXPLORER_LINK: Generate blockchain explorer links for transaction hashes
 * - CHECK_TX_CONFIRMATION: Check block confirmation status for transactions using viem
 * - FETCH_WITH_PAYMENT: Make paid requests to x402 APIs
 */
const cdpPlugin: Plugin = {
  name: "cdp",
  description:
    "Coinbase Developer Platform plugin providing authenticated EVM account creation, token transfers, NFT transfers, swaps, and x402 paid API requests via CDP SDK",
  evaluators: [],
  providers: [walletStateProvider],
  actions: [cdpWalletInfo, cdpWalletCheckBalance, cdpWalletTokenTransfer, cdpWalletNftTransfer, cdpWalletSwap, cdpResolveEns, cdpTxExplorerLink, cdpCheckTxConfirmation],
  services: [CdpService],
};

default cdpPlugin;



} // namespace elizaos
