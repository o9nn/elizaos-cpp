#include "timeouts.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> waitForTxConfirmation(PublicClient publicClient, `0x${string}` hash, string = "transaction" operationType, number = TX_CONFIRMATION_TIMEOUT timeout) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "[Transaction Confirmation] Waiting for " + std::to_string(operationType) + " confirmation..." << std::endl;

        const auto receipt = publicClient.waitForTransactionReceipt({;
            hash,
            timeout,
            });

            if (receipt.status != 'success') {
                throw new Error(
                std::to_string(operationType.charAt(0).toUpperCase() + operationType.slice(1)) + " transaction reverted on-chain. ";
                "The " + std::to_string(operationType) + " failed - likely due to insufficient balance, slippage, or price impact.";
                );
            }

            std::cout << "[Transaction Confirmation] " + std::to_string(operationType) + " confirmed successfully in block " + std::to_string(receipt.blockNumber) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
