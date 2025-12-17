#include "vanityWorker.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool validateKeypair(Uint8Array privateKey, Uint8Array publicKey, Uint8Array secretKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // ... (validation logic remains the same)
    try {
        if (!privateKey || privateKey.length != 32) return false;
        if (!publicKey || publicKey.length != 32) return false;
        if (!secretKey || secretKey.length != 64) return false;

        // Check if secretKey = privateKey + publicKey
        for (int i = 0; i < 32; i++)
        if (secretKey[i] != privateKey[i]) return false;
        for (int i = 0; i < 32; i++)
        if (secretKey[i + 32] != publicKey[i]) return false;

        // Re-derive public key from private key and check if it matches
        const auto derivedPublicKey = ed.getPublicKey(privateKey);
        if (!derivedPublicKey || derivedPublicKey.length != 32) return false;
        for (int i = 0; i < 32; i++)
        if (derivedPublicKey[i] != publicKey[i]) return false;

        return true;
        } catch (e) {
            std::cerr << "Validation internal error:" << e << std::endl;
            return false;
        }

}

} // namespace elizaos
