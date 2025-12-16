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




`
      } else if (endpoint === 'https://api.devnet.solana.com') {
        cluster = 'devnet'
      }
  
      return cluster ? `?cluster=${cluster}` : ''
    }
  
    return `https://explorer.solana.com/${itemType}/${viewTypeOrItemAddress}${getClusterUrlParam()}`
  }
} // namespace elizaos
