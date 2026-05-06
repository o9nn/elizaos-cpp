#include "db.hpp"
#include "tokenSupplyHelpers/customWallet.hpp"
#include "websocket-client.hpp"
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



std::future<std::string> claimFees(PublicKey nftMint, PublicKey poolId, Connection connection, PublicKey claimer, WebSocketClient websocket, Token token);

} // namespace elizaos
