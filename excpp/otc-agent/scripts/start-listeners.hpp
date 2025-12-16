#include ".src/services/token-registration-listener-base.hpp"
#include ".src/services/token-registration-listener-solana.hpp"
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

#!/usr/bin/env bun

/**
 * Listener Startup Script
 *
 * This script initializes blockchain event listeners for token registration.
 * It can be run manually or automatically during application startup.
 */

;
;

async 

  // Start Base listener
  try {
    console.log('📡 Starting Base listener...');
    await startBaseListener();
    console.log('✅ Base listener started\n');
  } catch (error) {
    console.error('❌ Failed to start Base listener:', error);
  }

  // Start Solana listener
  try {
    console.log('📡 Starting Solana listener...');
    await startSolanaListener();
    console.log('✅ Solana listener started\n');
  } catch (error) {
    console.error('❌ Failed to start Solana listener:', error);
  }

  console.log('🎯 All listeners initialized successfully!');
  console.log('\n📝 Available endpoints:');
  console.log('  • POST /api/listeners/start - Start specific listeners');
  console.log('  • POST /api/listeners/backfill - Backfill historical events');
  console.log('\n💡 Use Ctrl+C to stop all listeners');

  // Keep the process running
  process.on('SIGINT', () => {
    console.log('\n👋 Shutting down listeners...');
    process.exit(0);
  });

  process.on('SIGTERM', () => {
    console.log('\n👋 Shutting down listeners...');
    process.exit(0);
  });
}

// Run if called directly
if (import.meta.main) {
  main().catch(console.error);
}

default main;
} // namespace elizaos
