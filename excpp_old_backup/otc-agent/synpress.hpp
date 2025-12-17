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
 * Synpress + Playwright configuration for wallet-based E2E tests
 * 
 * These tests use real MetaMask wallets to test:
 * - Wallet connection flows
 * - Order creation with transaction signing
 * - Two-party trading (buyer + seller)
 * 
 * IMPORTANT: Run with `npx playwright test --config=synpress.config.ts`
 * The wallet setup is cached in .cache/synpress-cache to prevent re-setup loops.
 */
  
  // Run tests serially - wallet tests need isolation
  
  // Longer timeouts for wallet interactions
  
  // Fail fast in CI

    // Longer timeouts for wallet operations
    
    // Headed mode required for MetaMask
  
        // Use Chrome stable channel in CI for better extension support
        // Required for MetaMask extension
            // CI-specific flags for extension loading
  
  // Don't auto-start server - must be running already with contracts deployed

} // namespace elizaos
