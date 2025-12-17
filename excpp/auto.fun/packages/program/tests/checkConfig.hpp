#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// check-admin.ts

 // Import BN for handling large numbers

  // Set up the provider from the environment variables

  // Instantiate the program using the generated types

  // Derive the config PDA using the seed "config"

  // Derive the global WSOL account PDA using the seeds

  // The native mint account is provided in the IDL.

  // Derive the vault config PDA using the seed "raydium_vault_config"

  // Fetch the account info using the provider's connection.

  // Check each account individually and log if not found

    // Decode and log global vault account data if needed


} // namespace elizaos
