#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "db.hpp"
#include "tokenSupplyHelpers/customWallet.hpp"
#include "websocket-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<std::string> claimFees(PublicKey nftMint, PublicKey poolId, Connection connection, PublicKey claimer, WebSocketClient websocket, Token token);

} // namespace elizaos
