#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

struct EntityWalletResult {
    std::string walletAddress;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct EntityWalletError {
    ActionResult result;
};


using EntityWalletResponse = std::variant<EntityWalletResult, EntityWalletError>;

/**
 * Retrieves entity wallet information from runtime and validates it exists.
 * Returns either the wallet address on success, or a complete ActionResult on failure.
 */
std::future<EntityWalletResponse> getEntityWallet(IAgentRuntime runtime, Memory message, const std::string& actionName, std::optional<HandlerCallback> callback);,
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
          error: new Error("Entity not found"),
        },
      };
    }

    const walletEntityId = entity.metadata?.author_id;

    if (!walletEntityId) {
      const errorText = "Wallet entity ID not found in entity metadata.";

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

    const walletEntity = await runtime.getEntityById(walletEntityId);
    if (!walletEntity) {
      const errorText = "Wallet entity not found.";
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
    const walletAddress = walletEntity.metadata?.walletAddress as string;
    // Check if wallet already exists in entity metadata
    if (!walletAddress) {
      const errorText =
        "Unable to fetch user's wallet information. Please create a wallet first.";

      if (callback) {
        await callback({
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
          error: new Error("Wallet not found"),
        },
      };
    }

    if (!walletAddress) {
      const errorText = "Wallet address not found in entity metadata.";

      if (callback) {
        await callback({
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
          error: new Error("Wallet address not found"),
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
    logger.error("Error getting entity wallet address:", error instanceof Error ? error.message : String(error));

    const errorText = "Failed to retrieve wallet information.";

    if (callback) {
      await callback({
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
          error: error instanceof Error ? error.message : String(error),
        },
        error: error instanceof Error ? error : new Error(String(error)),
      },
    };
  }
}

} // namespace elizaos
