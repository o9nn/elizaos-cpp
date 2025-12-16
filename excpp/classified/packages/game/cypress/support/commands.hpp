#include "database-helpers.hpp"
#include "knowledge-helpers.hpp"
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

/// <reference types="cypress" />

/**
 * Custom Cypress Commands
 * Provides reusable commands for common test operations
 */

// Add custom command types
    struct Chainable {
    std::string text;
    std::string userId;
    std::string roomId;
    std::optional<std::string> messageId;
    std::string text;
    std::optional<std::string> userId;
    std::optional<std::string> roomId;
    std::optional<std::string> messageId;
};

// Initialize helpers

// Setup and Environment Commands

// Custom command to wait for backend to be ready

// Custom command to wait for elizaClient to be initialized

// Custom command to send message via API

    // Handle both string and object formats

    // Add API key if configured

// UI Helper Commands

// Knowledge Management Commands

// Capability Management Commands

// Database Helper Commands

// Export for use in tests

} // namespace elizaos
