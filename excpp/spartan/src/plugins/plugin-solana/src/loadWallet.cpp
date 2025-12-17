#include "loadWallet.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<WalletResult> loadWallet(AgentRuntime runtime, bool requirePrivateKey = true) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto teeMode = runtime.getSetting('TEE_MODE') || TEEMode.OFF;

        if (teeMode != TEEMode.OFF) {
            const auto walletSecretSalt = runtime.getSetting('WALLET_SECRET_SALT');
            if (!walletSecretSalt) {
                throw std::runtime_error('WALLET_SECRET_SALT required when TEE_MODE is enabled');
            }

            const auto deriveKeyProvider = new DeriveKeyProvider(teeMode);
            const auto deriveKeyResult = deriveKeyProvider.deriveEd25519Keypair(;
            '/',
            walletSecretSalt,
            runtime.agentId;
            );

            return requirePrivateKey;
        ? { signer: deriveKeyResult.keypair }
        : { address: deriveKeyResult.keypair.publicKey };
        }

        // TEE mode is OFF
        if (requirePrivateKey) {
            const auto privateKeyString =;
            runtime.getSetting('SOLANA_PRIVATE_KEY') || runtime.getSetting('WALLET_PRIVATE_KEY');

            if (!privateKeyString) {
                throw std::runtime_error('Private key not found in settings');
            }

            try {
                // First try base58
                const auto secretKey = bs58.decode(privateKeyString);
                return { signer: Keypair.fromSecretKey(secretKey) }
                } catch (e) {
                    std::cout << 'Error decoding base58 private key:' << e << std::endl;
                    try {
                        // Then try base64
                        std::cout << 'Try decoding base64 instead' << std::endl;
                        const auto secretKey = Uint8Array.from(Buffer.from(privateKeyString, 'base64'));
                        return { signer: Keypair.fromSecretKey(secretKey) }
                        } catch (e2) {
                            std::cerr << 'Error decoding private key: ' << e2 << std::endl;
                            throw std::runtime_error('Invalid private key format');
                        }
                    }
                    } else {
                        const auto publicKeyString =;
                        runtime.getSetting('SOLANA_PUBLIC_KEY') || runtime.getSetting('WALLET_PUBLIC_KEY');

                        if (!publicKeyString) {
                            throw std::runtime_error('Public key not found in settings');
                        }

                        return { address: new PublicKey(publicKeyString) }
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
