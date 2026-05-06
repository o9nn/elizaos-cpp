#include "bignumber.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

BigNumber toBN(const std::variant<std::string, double, BigNumber>& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            return new BigNumber(value);
            } catch (error) {
                throw std::runtime_error(`Failed to convert value to BigNumber: ${value}`);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string formatBN(BigNumber value, auto decimals) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            return value.dividedBy(new BigNumber(10).pow(decimals)).toFixed();
            } catch (error) {
                throw std::runtime_error(`Failed to format BigNumber: ${value}`);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
