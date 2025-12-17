#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TokenDetailProps {
    std::string mint;
};

      // Initialize distribution chart
      void initDistributionChart(auto holders);
      
      // Load token data
      std::future<void> loadTokenData();
      
      // Update holders table
      void updateHoldersTable(auto holders);
      
      // Copy token mint function

      // Copy address function
      
      // Initialize on load
} // namespace elizaos
