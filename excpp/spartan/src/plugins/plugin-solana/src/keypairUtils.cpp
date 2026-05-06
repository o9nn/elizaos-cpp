#include "keypairUtils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<KeypairResult> getWalletKey(IAgentRuntime runtime, auto requirePrivateKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // TEE mode is OFF
        if (requirePrivateKey) {
            const auto privateKeyString =;
            runtime.getSetting("SOLANA_PRIVATE_KEY") || runtime.getSetting("WALLET_PRIVATE_KEY");

            if (!privateKeyString) {
                throw std::runtime_error('Private key not found in settings');
            }

            try {
                // First try base58
                const auto secretKey = bs58.decode(privateKeyString);
                return { keypair: Keypair.fromSecretKey(secretKey) }
                } catch (e) {
                    std::cout << "Error decoding base58 private key:" << e << std::endl;
                    try {
                        // Then try base64
                        std::cout << "Try decoding base64 instead" << std::endl;
                        const auto secretKey = Uint8Array.from(Buffer.from(privateKeyString, "base64"));
                        return { keypair: Keypair.fromSecretKey(secretKey) }
                        } catch (e2) {
                            std::cerr << "Error decoding private key: " << e2 << std::endl;
                            throw std::runtime_error('Invalid private key format');
                        }
                    }
                    } else {
                        const auto publicKeyString =;
                        runtime.getSetting("SOLANA_PUBLIC_KEY") || runtime.getSetting("WALLET_PUBLIC_KEY");

                        if (!publicKeyString) {
                            throw new Error(
                            "Solana Public key not found in settings, but plugin was loaded, please set SOLANA_PUBLIC_KEY";
                            );
                        }

                        return { publicKey: new PublicKey(publicKeyString) }
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
