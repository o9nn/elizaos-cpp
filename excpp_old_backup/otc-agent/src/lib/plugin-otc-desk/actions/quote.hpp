#include ".providers/quote.hpp"
#include ".services/priceFeed.hpp"
#include "elizaos/core.hpp"
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

// quote action - generate a new OTC quote and return an XML object to the frontend

void parseQuoteRequest(const std::string& text); {

  // Parse token amount (various formats)

  // Parse discount (percentage or bps)
    // If it's a percentage (has % or "percent"), convert to bps

  // Parse payment currency

void parseNegotiationRequest(const std::string& text); {

  // Token amount (reuse existing regex)

  // Discount request

  // Lockup period

  // Payment currency

std::future<string | null> extractTokenContext(const std::string& text);

std::future<OTCConsignment | null> findSuitableConsignment(const std::string& tokenId, const std::string& tokenAmount, double discountBps, double lockupDays);

// Worst possible deal defaults (lowest discount, longest lockup)

    // Check for quote cancellation
        await deleteUserQuote(entityId);

    // Parse the request(s)

    // Fetch token info for dynamic symbol/name

      // Generate terms-only quote

        // Token metadata

    // ------------- Simple discount-based quote -------------

      // Token metadata


} // namespace elizaos
