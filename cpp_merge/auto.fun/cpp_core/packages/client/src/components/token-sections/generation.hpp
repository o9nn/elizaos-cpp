#pragma once
#include <algorithm>
#include <future>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "audio-player.hpp"
#include "button.hpp"
#include "copy-button.hpp"
#include "loader.hpp"

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
  // OAuth 2.0 (kept for potential std::future use or profile fetching)
  // OAuth 1.0a (Required for posting)

using PendingShare = {

// --- Expected API Response Types ---
struct TokenInfoResponse {
    std: name;
    std: ticker;
    std::optional<std:> description;
    std::optional<std:> image;
};


} // namespace elizaos
