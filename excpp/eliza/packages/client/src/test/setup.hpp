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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Test setup file for Bun:test

// Import React to access internal state

// Set up DOM environment with Happy DOM (recommended by Bun)

// Extend expect with jest-dom matchers

// Create a comprehensive localStorage mock

// Set up fresh localStorage and sessionStorage for each test
  // Create fresh storage instances for each test

  // Reset window dimensions to default

// Clean up after each test

  // Clear storage but don't reset the implementation

  // Force React to clear any cached hook state
  // This helps prevent cross-test contamination when running multiple test files

// React 19 specific setup - add missing APIs for testing-library compatibility

  // Add React.createRef polyfill for React 19 compatibility with testing-library

// Mock window.matchMedia

// Mock IntersectionObserver
    void observe();
    void disconnect();
    void unobserve();
    void takeRecords();

// Mock ResizeObserver
    void observe();
    void disconnect();
    void unobserve();

// Mock scrollTo for window and elements

// Add any other global test setup here

} // namespace elizaos
