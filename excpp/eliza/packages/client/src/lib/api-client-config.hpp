#include "elizaos/api-client.hpp"
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

`;
  const apiKey = localStorage.getItem(getLocalStorageApiKey());

  const config: ApiClientConfig = {
    baseUrl: window.location.origin,
    timeout: 30000,
    headers: {
      Accept: 'application/json',
    },
  };

  // Only include apiKey if it exists (don't pass undefined)
  if (apiKey) {
    config.apiKey = apiKey;
  }

  return config;
}

// Singleton instance
let elizaClientInstance: ElizaClient | null = null;


  return elizaClientInstance;
}



// Function to reset the singleton (useful for API key changes)


`;

  if (newApiKey) {
    localStorage.setItem(getLocalStorageApiKey(), newApiKey);
  } else {
    localStorage.removeItem(getLocalStorageApiKey());
  }

  // Reset the singleton so it uses the new API key
  resetElizaClient();
}

} // namespace elizaos
