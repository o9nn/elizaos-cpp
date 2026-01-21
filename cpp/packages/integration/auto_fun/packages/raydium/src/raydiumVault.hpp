#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "pdas.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<void> depositToRaydiumVault(anchor::AnchorProvider provider, anchor.web3.Keypair signerWallet, Program<RaydiumVault> program, anchor.web3.PublicKey position_nft, anchor.web3.PublicKey claimer_address);

std::future<void> changeClaimer(Program<RaydiumVault> program, anchor.web3.Keypair signerWallet, anchor.web3.PublicKey position_nft, anchor.web3.PublicKey new_claimer_address);

std::future<void> emergencyWithdraw(Program<RaydiumVault> program, anchor.web3.Keypair signerWallet, anchor.web3.PublicKey position_nft);

std::future<void> claim(Program<RaydiumVault> program, anchor.web3.Keypair signerWallet, anchor.web3.PublicKey position_nft, anchor.web3.PublicKey poolId, anchor.web3.Connection connection, anchor.web3.PublicKey claimer, Token token);

std::future<void> checkBalance(anchor.web3.Connection connection, anchor.web3.Keypair signerWallet, anchor.web3.PublicKey position_nft, anchor.web3.PublicKey claimer_address);

} // namespace elizaos
