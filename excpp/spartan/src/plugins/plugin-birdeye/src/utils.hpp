#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Constants

// Chain abbreviations and alternative names mapping
    // Solana

    // Ethereum

    // Arbitrum

    // Avalanche

    // BSC

    // Optimism

    // Polygon

    // Base
    // no common abbreviations

    // zkSync

    // Sui
    // no common abbreviations

class BirdeyeApiError {
public:
    BirdeyeApiError(const std::string& message);
};

// Time-related types and constants

using TimeUnit = keyof typeof TIME_UNITS;
using Timeframe = keyof typeof TIMEFRAME_KEYWORDS;

// Helper functions
    // Check for SUI address (0x followed by 64 hex chars)
    // Check for EVM address (0x followed by 40 hex chars)
    // Default to solana

    // EVM-compatible chains (Ethereum, Arbitrum, Avalanche, BSC, Optimism, Polygon, Base, zkSync)

    // Solana addresses (base58 strings)

    // Sui addresses (0x followed by 64 hex chars)

// Time extraction and analysis
    // First, check for explicit timeframe mentions

    // Check for semantic timeframe hints

    // Analyze for time-related words

    // Default based on context

    // Check for specific date ranges

    // Check for relative time expressions

    // Check for semantic time ranges

    // Analyze context for appropriate default range

    // Default to last 24 hours

    // Check for explicit limit mentions

    // Check for semantic limit hints

    // Default based on context

// Formatting helpers

// API helpers

// Formatting helpers

    // Add metadata if available

// Extract symbols from text
    // loose mode will try to extract more symbols but may include false positives
    // strict mode will only extract symbols that are clearly formatted as a symbol using $SOL format

    // Match patterns - this may
                  // $SYMBOL format
                  // $SYMBOL format with lowercase
                  // $SYMBOL format
                  // After articles (a/an)
                  // // Standalone caps
                  // // Quoted symbols
                  // // Common price patterns

    // Extract all matches

    // Basic Information

    // Social Links

    // Logo


} // namespace elizaos
