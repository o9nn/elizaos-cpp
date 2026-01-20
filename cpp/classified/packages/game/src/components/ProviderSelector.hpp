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


struct Provider {
    std::string name;
    std::string display_name;
    bool enabled;
    bool requires_api_key;
    'available' | 'not_configured' | 'error' status;
    std::string message;
};

struct ProviderStatus {
    std::vector<Provider> providers;
    std::string active;
    string | null selected;
    std::vector<std::string> preferences;
};

struct ProviderResponse {
    bool success;
    ProviderStatus data;
};

struct ProviderSelectorProps {
    std::optional<(provider: string) => void> onProviderChange;
};

  // Check if we're in Tauri environment
        await import('@tauri-apps/api/core');

  // Fetch provider status

  // Set selected provider

        await fetchProviders(); // Refresh status

      // Poll for updates every 30 seconds


} // namespace elizaos
