#include ".audio-player.hpp"
#include ".button.hpp"
#include ".copy-button.hpp"
#include ".loader.hpp"
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



// --- API Base URL ---

// Function to get the correct icon path based on the current tab
    // Audio tab

// Additional imports for balance checking

// Storage keys for Twitter auth

// Types for Twitter authentication
using TwitterCredentials = {
  // OAuth 2.0 (kept for potential future use or profile fetching)
  // OAuth 1.0a (Required for posting)

using PendingShare = {

// --- Expected API Response Types ---
struct TokenInfoResponse {
    std::string name;
    std::string ticker;
    std::optional<std::string> description;
    std::optional<std::string> image;
};


} // namespace elizaos
