#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


struct Provider {
    std::string name;
    std::string display_name;
    bool enabled;
    bool requires_api_key;
    std::string status;
    std::string message;
};

struct ProviderStatus {
    std::vector<Provider> providers;
    std::string active;
    std::string selected;
    std::vector<std::string> preferences;
};

struct ProviderResponse {
    bool success;
    ProviderStatus data;
};

struct ProviderSelectorProps {
};

  // Check if we're in Tauri environment

  // Fetch provider status

  // Set selected provider

      // Poll for updates every 30 seconds


} // namespace elizaos
