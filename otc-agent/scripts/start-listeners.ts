#!/usr/bin/env bun

/**
 * Listener Startup Script
 *
 * This script initializes blockchain event listeners for token registration.
 * It can be run manually or automatically during application startup.
 */

import { startBaseListener, backfillBaseEvents } from '../src/services/token-registration-listener-base';
import { startSolanaListener, backfillSolanaEvents } from '../src/services/token-registration-listener-solana';

async function main() {
  console.log('🚀 Starting token registration listeners...\n');

  // Check environment variables
  const requiredEnvVars = [
    'NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS',
    'NEXT_PUBLIC_BASE_RPC_URL',
    'NEXT_PUBLIC_SOLANA_PROGRAM_ID',
    'NEXT_PUBLIC_SOLANA_RPC'
  ];

  const missingVars = requiredEnvVars.filter(varName => !process.env[varName]);

  if (missingVars.length > 0) {
    console.warn('⚠️  Missing environment variables:', missingVars);
    console.log('Some listeners may not start properly.\n');
  }

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

export default main;