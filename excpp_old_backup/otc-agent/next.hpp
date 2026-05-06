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



  // Skip TypeScript type checking during build (webpack build succeeds, types can be fixed separately)
  // Explicitly set workspace root to prevent lockfile detection warnings
  // Don't transpile ox - it has compiled .js files in _esm/ and _cjs/
  // Instead, we'll configure webpack to prefer .js files and exclude .ts files from ox
  // Skip image optimization for external images - they come from too many sources
  // Vercel Blob caching handles performance for Solana token logos
  // Fix cross-origin chunk loading issues
  // Supports localhost development and Cloudflare tunnels out of the box
        // Common localhost patterns (works for all developers)
        
        // Cloudflare tunnel support (set TUNNEL_DOMAIN in .env.local)
        
        // Allow custom origins via environment variable (comma-separated)
    // Ignore handlebars require.extensions warning
    
    // Exclude TypeScript source files from node_modules/ox/ from being processed
    // porto imports from ox without extensions, webpack resolves to .ts files
    // We need to prevent Next.js loaders from processing these files
    
    // Add rule BEFORE other rules to exclude .ts files from ox
    // This prevents Next.js TypeScript loaders from processing them
    
    // Exclude Web3 packages and handlebars from server-side bundling
    
    // Configure webpack to ignore files that shouldn't trigger rebuilds

      // Prevent webpack from processing .ts files from ox package
      // porto imports from ox without extensions, webpack resolves to .ts files
      // ox has compiled .js files that should be used via package.json exports
          // Ignore .ts files from ox package - use compiled .js files instead
      // Ignore IndexedDB related imports on server
    // Return modified config
          // Force all packages to use root-level viem and ox (fix @base-org/account nested deps)
          // Redirect ox TypeScript imports to use package.json exports (compiled .js files)
          // This prevents webpack from processing .ts source files
        // Prefer .js files over .ts files for node_modules to avoid processing TypeScript source
        // This ensures webpack resolves ox imports to .js files instead of .ts files
          // Only include .ts/.tsx after .js so webpack prefers compiled files
        // Ensure nested packages can resolve @noble/hashes from top-level
          // Add parent node_modules for nested package resolution
  // This is required to support PostHog trailing slash API requests


} // namespace elizaos
