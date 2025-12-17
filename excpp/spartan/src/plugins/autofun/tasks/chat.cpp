#include "chat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Uint8Array decodeBase58(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto ALPHABET = '123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz';
        const auto ALPHABET_MAP = new Map(ALPHABET.split('').map((c, i) => [c, BigInt(i)]));

        auto result = BigInt(0);
        for (const auto& char : str)
            const auto value = ALPHABET_MAP.get(char);
            if (value == undefined) {
                throw std::runtime_error('Invalid base58 character');
            }
            result = result * BigInt(58) + value;
        }

        const auto bytes = [];
        while (result > 0n) {
            bytes.unshift(Number(result & 0xffn));
            result = result >> 8n;
        }

        // Add leading zeros
        for (int i = 0; i < str.length && str[i] === '1'; i++) {
            bytes.unshift(0);
        }

        return new Uint8Array(bytes);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
