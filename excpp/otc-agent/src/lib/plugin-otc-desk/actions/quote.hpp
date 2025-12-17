#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".providers/quote.hpp"
#include ".services/priceFeed.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// quote action - generate a new OTC quote and return an XML object to the frontend


  // Parse token amount (various formats)

  // Parse discount (percentage or bps)
    // If it's a percentage (has % or "percent"), convert to bps

  // Parse payment currency


  // Token amount (reuse existing regex)

  // Discount request

  // Lockup period

  // Payment currency

std::future<std::string> extractTokenContext(const std::string& text);

// Worst possible deal defaults (lowest discount, longest lockup)

    // Check for quote cancellation

    // Parse the request(s)

    // Fetch token info for dynamic symbol/name

      // Generate terms-only quote

        // Token metadata

    // ------------- Simple discount-based quote -------------

      // Token metadata


} // namespace elizaos
