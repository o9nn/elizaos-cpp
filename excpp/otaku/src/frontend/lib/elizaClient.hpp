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

const elizaClient = ElizaClient.create({
  baseUrl: window.location.origin,
  timeout: 30000,
  headers: {
    'Accept': 'application/json',
  },
  apiKey: localStorage.getItem('eliza-api-key') || undefined,
});

 else {
    localStorage.removeItem('eliza-api-key');
  }
}


} // namespace elizaos
