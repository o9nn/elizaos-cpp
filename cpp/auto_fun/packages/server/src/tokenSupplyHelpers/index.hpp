#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "db.hpp"
#include "externalToken.hpp"
#include "redis.hpp"
#include "util.hpp"
#include "websocket-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define max swaps to keep in Redis list (consistent with other files)

std::future<void> getAllLockedTokens();

std::future<void> handleSignature(const std::string& signature, const std::any& token, double solPriceUSD);

std::future<void> processSwapLog(const std::any& token, const std::string& signature, double solPriceUSD, const std::vector<std::string>& logs);

bool shouldUpdateSupply(const std::any& token);

  // retry in case it fails once

std::future<bool> isValidSwapTx(Connection connection, const std::string& signature, const std::string& mint);

std::future<void> processLastValidSwap(const std::any& token, double solPriceUSD, auto limit);

std::future<double> updateHoldersCache(const std::string& mint, bool imported = false);

// Function to process a token update and emit WebSocket events
std::future<void> processTokenUpdateEvent(const std::any& tokenData, bool shouldEmitGlobal = false, bool isNewTokenEvent = false);

} // namespace elizaos
