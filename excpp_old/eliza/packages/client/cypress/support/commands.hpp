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

/// <reference types="cypress" />
/// <reference types="@testing-library/cypress" />

// Custom command to wait for the app to be ready
  // Wait for the root element to exist

  // Wait for the app to be interactive

  // Wait a bit for React to hydrate and render

  // Check if there's any loading indicator and wait for it to disappear

// Custom command to login (can be implemented based on your auth flow)
  // This is a placeholder - implement based on your auth system

  // Wait for redirect or auth token

// Custom command to connect to WebSocket
    // Wait for WebSocket connection to be established
          // Check if socket exists on window or in your app state

// Custom command to clean up test data
  // Clean up any test data created during tests
    // Clear local storage

    // Clear session storage

    // Clear cookies

// Utility command to select by test id

// Utility command to wait for API response

// Add TypeScript support for custom commands
    struct Chainable {
};


} // namespace elizaos
