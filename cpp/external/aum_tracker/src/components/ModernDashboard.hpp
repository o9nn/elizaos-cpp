#pragma once
#include <algorithm>
#include <any>
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



struct DashboardProps {
    std::optional<std::any> initialData;
};

      // Initialize performance chart
      void initPerformanceChart();

      // Update dashboard with data
      std::future<void> updateDashboard(auto data);

      // Update performance chart
      void updatePerformanceChart();

      // Load assets data
      std::future<void> loadAssets();

      // Display assets in table
      void displayAssets(auto assets);

      // Create asset table row
      void createAssetRow(auto asset);

      // Load wallets data
      std::future<void> loadWallets();

      // Display wallets in table
      void displayWallets(auto wallets);

      // Create wallet table row
      void createWalletRow(auto wallet);

      // Copy address to clipboard
        // Could add a toast notification here

      // Filter assets

      // Filter wallets

      // Check for existing running refresh jobs
      std::future<void> checkForRunningJobs();

      // Lightweight refresh - just fetch current data without triggering prefetch
      std::future<void> lightweightRefresh();

      // Initialize on load

        // Auto-refresh every 5 minutes (lightweight)

} // namespace elizaos
