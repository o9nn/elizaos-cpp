#include "elizaos/core.hpp"
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

;
;
;

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

    try {
      // First try base58
      const secretKey = bs58.decode(privateKeyString);
      return { keypair: Keypair.fromSecretKey(secretKey) };
    } catch (e) {
      logger.log('Error decoding base58 private key:', e);
      try {
        // Then try base64
        logger.log('Try decoding base64 instead');
        const secretKey = Uint8Array.from(Buffer.from(privateKeyString, 'base64'));
        return { keypair: Keypair.fromSecretKey(secretKey) };
      } catch (e2) {
        logger.error('Error decoding private key: ', e2);
        throw new Error('Invalid private key format');
      }
    }
  } else {
    const publicKeyString =
      runtime.getSetting('SOLANA_PUBLIC_KEY') ?? runtime.getSetting('WALLET_PUBLIC_KEY');

    if (!publicKeyString) {
      throw new Error(
        'Solana Public key not found in settings, but plugin was loaded, please set SOLANA_PUBLIC_KEY'
      );
    }

    return { publicKey: new PublicKey(publicKeyString) };
  }
}

} // namespace elizaos
