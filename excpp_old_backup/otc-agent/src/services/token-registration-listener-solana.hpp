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
 * Start listening for register_token events from Solana program
 */
std::future<void> startSolanaListener();

/**
 * Handle program logs
 */
std::future<void> handleProgramLogs(Logs logs);

/**
 * Parse register_token transaction
 */
std::future<void> parseRegisterTokenTransaction(VersionedTransactionResponse tx);

/**
 * Backfill historical events (run once after deployment)
 */
std::future<void> backfillSolanaEvents(std::optional<std::vector<std::string>> signatures);

} // namespace elizaos
