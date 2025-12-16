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
import type { RaydiumVault } from '../target/types/raydium_vault';

(async () => {
  // Set up the provider from the environment variables
  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);

  // Instantiate the raydium vault program using the generated types
  const raydiumProgram = anchor.workspace.RaydiumVault as Program<RaydiumVault>;

  // Derive the vault config PDA using the seed "raydium_vault_config"
  const [vaultConfigPDA] = anchor.web3.PublicKey.findProgramAddressSync(
    [Buffer.from("raydium_vault_config")],
    raydiumProgram.programId
  );

  // Define the new executor authority public key
  const newExecutorAuthority = new anchor.web3.PublicKey('autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S'); // Replace with the actual new executor public key

  // Call the change_executor_authority ).rpc();

  console.log("Transaction sent successfully!");
  console.log("Signature:", txSignature);
  } catch (error) {
    console.error("Error changing executor authority:", error);
  }
})();
} // namespace elizaos
