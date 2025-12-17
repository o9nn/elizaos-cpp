#include "errors.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ErrorResponse handleError(const std::any& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        logger.error(
        "Clanker plugin error: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
        );

        if (error instanceof ClankerError) {
            return error.toResponse();
        }

        if (error instanceof Error) {
            // Check for common errors
            if (error.message.includes("insufficient funds")) {
                return new ClankerError(;
                ErrorCode.INSUFFICIENT_BALANCE,
                "Insufficient balance to complete transaction",
                { originalError: error.message },
                [;
                "Check your wallet balance",
                "Ensure you have enough ETH for gas fees",
                ],
                ).toResponse();
            }

            if (
            error.(std::find(message.begin(), message.end(), "network") != message.end()) ||;
            error.(std::find(message.begin(), message.end(), "connection") != message.end());
            ) {
                return new ClankerError(;
                ErrorCode.NETWORK_ERROR,
                "Network connection error",
                { originalError: error.message },
                ["Check your internet connection", "Verify RPC endpoint is accessible"],
                ).toResponse();
            }

            if (error.message.includes("slippage")) {
                return new ClankerError(;
                ErrorCode.SLIPPAGE_EXCEEDED,
                "Transaction would exceed slippage tolerance",
                { originalError: error.message },
                ["Try increasing slippage tolerance", "Wait for lower volatility"],
                ).toResponse();
            }

            if (error.message.includes("reverted")) {
                return new ClankerError(;
                ErrorCode.TRANSACTION_FAILED,
                "Transaction reverted on chain",
                { originalError: error.message },
                [;
                "Check transaction parameters",
                "Ensure contract state allows this operation",
                ],
                ).toResponse();
            }
        }

        // Generic error
        return new ClankerError(;
        ErrorCode.PROTOCOL_ERROR,
        "An unexpected error occurred",
        { originalError: std::to_string(error) },
        ["Please try again", "Contact support if the issue persists"],
        ).toResponse();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

bool validateAddress(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return /^0x[a-fA-F0-9]{40}$/.test(address);

}

bool validateAmount(const std::string& amount) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto num = BigInt(amount);
        return num >= 0n;
        } catch {
            return false;
        }

}

} // namespace elizaos
