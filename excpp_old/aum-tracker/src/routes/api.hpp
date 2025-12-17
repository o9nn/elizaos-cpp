#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".scripts/prefetch.hpp"
#include ".services/background/token-metadata.hpp"
#include ".services/csv.hpp"
#include ".services/database.hpp"
#include ".services/solana.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Global prefetch service instance to track running state

// Middleware

// Helper function to handle errors

// Helper function to validate wallet address

// Portfolio endpoints

    return handleError(error, c);

    // Calculate data freshness

    return handleError(error, c);

    // Calculate performance metrics

    return handleError(error, c);

    return handleError(error, c);

    // TODO: Implement timeframe-specific PNL calculation
    // This would require historical snapshots and price data

    return handleError(error, c);

// Wallet endpoints

    return handleError(error, c);

    return handleError(error, c);

    return handleError(error, c);

    return handleError(error, c);

    // TODO: Implement timeframe-specific wallet PNL calculation

    return handleError(error, c);

// Token endpoints

    return handleError(error, c);

    return handleError(error, c);

    return handleError(error, c);

    // Get SOL price from database

    // Aggregate tokens across all wallets

    // Add price data and update metadata from current token_prices table
        // Update symbol and name with current metadata from token_prices table

    return handleError(error, c);

    return handleError(error, c);

    // Find token price info

    // Find all wallets holding this token

    // Get token PNL data

    return handleError(error, c);

// Token metadata service endpoints

    return handleError(error, c);

    return handleError(error, c);

    return handleError(error, c);

    return handleError(error, c);

    return handleError(error, c);

// Admin endpoints
    // Check if refresh is already running

    // Parse query parameters

    // Start refresh process in background

    // Run refresh in background without blocking the response
        // Clear the global service when done

    return handleError(error, c);

// Admin refresh status endpoint

    return handleError(error, c);

    return handleError(error, c);

    return handleError(error, c);

// Health endpoint

    return handleError(error, c);


} // namespace elizaos
