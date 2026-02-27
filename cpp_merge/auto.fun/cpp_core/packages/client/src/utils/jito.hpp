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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// 

using JitoRegion = std::string;

void getJitoEndpoint(JitoRegion region);
/**
 * Send a transaction using Jito. This only supports sending a single transaction on mainnet only.
 * See https://jito-labs.gitbook.io/mev/searcher-resources/json-rpc-api-reference/transactions-endpoint/sendtransaction.
 * @param args.serialisedTx - A single transaction to be sent, in serialised form
 * @param args.region - The region of the Jito endpoint to use
 */
std::future<void> sendTxUsingJito(auto region);

} // namespace elizaos
