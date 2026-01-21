#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Services

// Actions
// 
// 

// 
// 

// Providers

// Types

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


} // namespace elizaos
