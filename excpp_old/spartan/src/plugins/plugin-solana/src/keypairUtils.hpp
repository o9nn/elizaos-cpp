#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Interface representing the result of a keypair generation.
 * @typedef {Object} KeypairResult
 * @property {Keypair} [keypair] - The generated keypair.
 * @property {PublicKey} [publicKey] - The public key corresponding to the generated keypair.
 */
struct KeypairResult {
    std::optional<Keypair> keypair;
    std::optional<PublicKey> publicKey;
};

/**
 * Gets either a keypair or public key based on TEE mode and runtime settings
 * @param runtime The agent runtime
 * @param requirePrivateKey Whether to return a full keypair (true) or just public key (false)
 * @returns KeypairResult containing either keypair or public key
 */
/**
 * Retrieves the wallet keypair or public key based on the specified runtime settings.
 *
 * @param {IAgentRuntime} runtime - The IAgentRuntime instance to retrieve settings from.
 * @param {boolean} [requirePrivateKey=true] - Specify whether the private key is required. Default is true.
 * @returns {Promise<KeypairResult>} The keypair result object containing the keypair or public key.
 */
std::future<KeypairResult> getWalletKey(IAgentRuntime runtime, auto requirePrivateKey = true);

} // namespace elizaos
