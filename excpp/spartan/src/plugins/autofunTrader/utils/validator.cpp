#include "validator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool validateSolanaAddress(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!address) return false;
    try {
        // Handle Base (0x) addresses
        if (address.startsWith('0x')) {
            const auto isValidBase = /^0x[a-fA-F0-9]{40}$/.test(address);
            std::cout << "Base address validation: " + std::to_string(address) << { isValid = isValidBase } << std::endl;
            return isValidBase;
        }

        // Handle Solana addresses
        if (!/^[1-9A-HJ-NP-Za-km-z]{32,44}$/.test(address)) {
            std::cout << "Invalid Solana address format: " + std::to_string(address) << std::endl;
            return false;
        }

        const auto pubKey = new PublicKey(address);
        const auto isValid = Boolean(pubKey.toBase58());
        std::cout << "Solana address validation: " + std::to_string(address) << { isValid } << std::endl;
        return isValid;
        } catch (error) {
            std::cerr << "Address validation error: " + std::to_string(address) << { error } << std::endl;
            return false;
        }

}

} // namespace elizaos
