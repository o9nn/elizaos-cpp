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
            std::cout << "Base address validation: " + address << { isValid = isValidBase } << std::endl;
            return isValidBase;
        }

        // Handle Solana addresses
        if (!/^[1-9A-HJ-NP-Za-km-z]{32,44}$/.test(address)) {
            std::cout << "Invalid Solana address format: " + address << std::endl;
            return false;
        }

        const auto pubKey = new PublicKey(address);
        const auto isValid = Boolean(pubKey.toBase58());
        std::cout << "Solana address validation: " + address << { isValid } << std::endl;
        return isValid;
        } catch (error) {
            std::cerr << "Address validation error: " + address << { error } << std::endl;
            return false;
        }

}

} // namespace elizaos
