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

/**
 * E2E Support File
 *
 * This file is loaded automatically before E2E test files.
 * Use it to set up global configuration and custom commands.
 */

// ***********************************************************
// This file is processed and loaded automatically before your test files.
// You can change the location of this file or turn off processing using the
// 'supportFile' config option.
// ***********************************************************

// Import commands (shared with component tests)

// Import Testing Library Cypress commands

// E2E-specific configurations
  // Prevent Cypress from failing the test on uncaught exceptions
  // This is useful for E2E tests where third-party scripts might throw errors

// Custom E2E commands
    struct Chainable {
};

// Login command
  // Check if login is required

// Wait for app to be ready
  // Wait for any loading indicators to disappear

  // Ensure the app container is visible

  // Wait a bit for any animations

// Navigate to agent

// Send chat message
  // Find and type in the input

  // Send the message

  // Wait for the message to appear

  // Wait for agent response

// Clear app data
    // Clear local storage

    // Clear session storage

    // Clear cookies

    // Note: IndexedDB clearing is commented out due to TypeScript compatibility issues
    // If your app uses IndexedDB, you may need to add custom clearing logic here
    // Example:
    // cy.window().its('indexedDB').invoke('deleteDatabase', 'your-db-name');

// E2E-specific viewport settings
  // Set a consistent viewport for E2E tests

// Screenshot on failure
  // Take a screenshot when a test fails

/**
 * E2E TESTING UTILITIES
 *
 * These commands help with common E2E testing scenarios:
 *
 * 1. LOGIN FLOWS
 *    - cy.login() - Handle authentication
 *    - Supports different auth methods
 *
 * 2. NAVIGATION
 *    - cy.navigateToAgent() - Go to agent chat
 *    - cy.waitForApp() - Wait for app ready
 *
 * 3. INTERACTIONS
 *    - cy.sendChatMessage() - Send and verify messages
 *    - Handles async responses
 *
 * 4. STATE MANAGEMENT
 *    - cy.clearAppData() - Reset application state
 *    - Clears all storage types
 *
 * BEST PRACTICES:
 * - Use these commands for consistency
 * - Add new commands as patterns emerge
 * - Keep commands focused and reusable
 */

} // namespace elizaos
