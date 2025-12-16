#include "db.hpp"
#include "tokenSupplyHelpers/customWallet.hpp"
#include "websocket-client.hpp"
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
;
;
;
;
;
;
;

const raydium_vault_IDL: RaydiumVault = JSON.parse(
  JSON.stringify(raydium_vault_IDL_JSON)
);

std::future<std::string> claimFees(PublicKey nftMint, PublicKey poolId, Connection connection, PublicKey claimer, WebSocketClient websocket, Token token); catch (error) {
      console.error("Error during claim attempt:", error);
      // attempt++;
    }
    // }
    if (!txSignature) {
      throw new Error("Failed to claim after multiple attempts.");
    }
    websocket.to(`claimer:${claimer.toBase58()}`).emit("claim", {
      txSignature,
      poolId: poolId.toBase58(),
      claimer: claimer.toBase58(),
    });
    console.log("Transaction Signature:", txSignature);
    return txSignature;
  } catch (error) {
    console.error("Error during claim:", error);
    throw error;
  }
}

} // namespace elizaos
