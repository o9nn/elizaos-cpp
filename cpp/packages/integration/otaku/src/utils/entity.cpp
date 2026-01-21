#include "entity.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<EntityWalletResponse> getEntityWallet(IAgentRuntime runtime, Memory message, const std::string& actionName, std::optional<HandlerCallback> callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto entityId = message.entityId;
            const auto entity = (runtime.getEntityById(entityId));

            if (!entity) {
                const auto errorText = "Unable to fetch entity information. Please try again.";

                if (callback) {
                    callback({
                        text: errorText,
                        content: { error: "Entity not found" },
                        });
                    }

                    return {
                        success: false,
                        result: {
                            text: errorText,
                            success: false,
                            values: { walletCreated: false, error: true },
                            data: {
                                actionName,
                                error: "Entity not found",
                                },
                                error: std::runtime_error("Entity not found"),
                                },
                                };
                            }

                            const auto walletEntityId = entity.metadata.author_id;

                            if (!walletEntityId) {
                                const auto errorText = "Wallet entity ID not found in entity metadata.";

                                return {
                                    success: false,
                                    result: {
                                        text: errorText,
                                        success: false,
                                        values: { walletCreated: false, error: true },
                                        data: { error: "Wallet entity ID not found" },
                                        },
                                        };
                                    }

                                    const auto walletEntity = runtime.getEntityById(walletEntityId);
                                    if (!walletEntity) {
                                        const auto errorText = "Wallet entity not found.";
                                        return {
                                            success: false,
                                            result: {
                                                text: errorText,
                                                success: false,
                                                values: { walletCreated: false, error: true },
                                                data: { error: "Wallet entity not found" },
                                                },
                                                };
                                            }
                                            const auto walletAddress = walletEntity.metadata.walletAddress;
                                            // Check if wallet already exists in entity metadata
                                            if (!walletAddress) {
                                                const auto errorText =;
                                                "Unable to fetch user's wallet information. Please create a wallet first.";

                                                if (callback) {
                                                    callback({
                                                        text: errorText,
                                                        content: { error: "Wallet not found" },
                                                        });
                                                    }

                                                    return {
                                                        success: false,
                                                        result: {
                                                            text: errorText,
                                                            success: false,
                                                            values: { walletCreated: false, error: true },
                                                            data: {
                                                                actionName,
                                                                error: "Wallet not found",
                                                                },
                                                                error: std::runtime_error("Wallet not found"),
                                                                },
                                                                };
                                                            }

                                                            if (!walletAddress) {
                                                                const auto errorText = "Wallet address not found in entity metadata.";

                                                                if (callback) {
                                                                    callback({
                                                                        text: errorText,
                                                                        content: { error: "Wallet address not found" },
                                                                        });
                                                                    }

                                                                    return {
                                                                        success: false,
                                                                        result: {
                                                                            text: errorText,
                                                                            success: false,
                                                                            values: { walletCreated: false, error: true },
                                                                            data: {
                                                                                actionName,
                                                                                error: "Wallet address not found",
                                                                                },
                                                                                error: std::runtime_error("Wallet address not found"),
                                                                                },
                                                                                };
                                                                            }

                                                                            return {
                                                                                success: true,
                                                                                walletAddress,
                                                                                metadata: {
                                                                                    walletAddress,
                                                                                    walletEntityId,
                                                                                    accountName: walletEntityId
                                                                                    },
                                                                                    };
                                                                                    } catch (error) {
                                                                                        std::cerr << "Error getting entity wallet address:" << true /* instanceof check */ ? error.message : std::to_string(error) << std::endl;

                                                                                        const auto errorText = "Failed to retrieve wallet information.";

                                                                                        if (callback) {
                                                                                            callback({
                                                                                                text: errorText,
                                                                                                content: { error: "Wallet retrieval failed" },
                                                                                                });
                                                                                            }

                                                                                            return {
                                                                                                success: false,
                                                                                                result: {
                                                                                                    text: errorText,
                                                                                                    success: false,
                                                                                                    values: { walletCreated: false, error: true },
                                                                                                    data: {
                                                                                                        actionName,
                                                                                                        error: true /* instanceof check */ ? error.message : std::to_string(error),
                                                                                                        },
                                                                                                        error: true /* instanceof check */ ? error : std::runtime_error(std::to_string(error)),
                                                                                                        },
                                                                                                        };
                                                                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
