import { defineConfig, devices } from '@playwright/test';

const OTC_DESK_PORT = parseInt(process.env.OTC_DESK_PORT || '4444');
const BASE_URL = `http://localhost:${OTC_DESK_PORT}`;

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
export default defineConfig({
  testDir: './tests/synpress',
  testMatch: /.*\.(test|spec)\.ts$/,
  
  // Run tests serially - wallet tests need isolation
  fullyParallel: false,
  workers: 1,
  
  // Longer timeouts for wallet interactions
  timeout: 180000,
  expect: {
    timeout: 30000,
  },
  
  // Fail fast in CI
  forbidOnly: !!process.env.CI,
  retries: 0, // No retries for wallet tests - each retry resets wallet state
  
  reporter: process.env.CI ? [['html'], ['github']] : 'list',
  
  use: {
    baseURL: BASE_URL,
    trace: 'retain-on-failure',
    screenshot: 'only-on-failure',
    video: 'retain-on-failure',
    
    // Longer timeouts for wallet operations
    actionTimeout: 60000,
    navigationTimeout: 60000,
    
    // Headed mode required for MetaMask
    headless: false,
  },
  
  projects: [
    {
      name: 'chromium-synpress',
      use: { 
        ...devices['Desktop Chrome'],
        // Use Chrome stable channel in CI for better extension support
        channel: process.env.CI ? 'chrome' : undefined,
        // Required for MetaMask extension
        launchOptions: {
          headless: false,
          args: [
            '--disable-web-security',
            '--disable-features=IsolateOrigins,site-per-process',
            // CI-specific flags for extension loading
            '--no-sandbox',
            '--disable-setuid-sandbox',
            '--disable-dev-shm-usage',
            '--disable-gpu',
            '--disable-software-rasterizer',
            '--allow-insecure-localhost',
          ],
        },
      },
    },
  ],
  
  // Don't auto-start server - must be running already with contracts deployed
  webServer: undefined,
});
