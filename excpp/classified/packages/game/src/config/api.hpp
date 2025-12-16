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

/**
 * API configuration for direct backend connections
 * This replaces the Vite proxy setup for better Tauri compatibility
 */

// Determine the backend URL based on environment
  // Get backend port from environment variables (with fallback to 7777)
    // Check multiple environment variable names

  // Check if we're in a browser environment
    // In Tauri, we always connect directly to the backend

    // In development (Vite dev server), connect directly to the backend

    // In production, use the same origin (backend serves the frontend)

  // In Node.js environment, connect directly to the backend server

/**
 * Helper function to construct API URLs
 * @param path - API path (e.g., '/api/setup/status')
 * @returns Full URL for the API endpoint
 */
  // Ensure path starts with /

/**
 * Fetch wrapper with proper error handling and default options
 */


} // namespace elizaos
