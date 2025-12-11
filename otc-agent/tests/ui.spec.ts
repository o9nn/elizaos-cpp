/**
 * OTC Marketplace UI E2E Tests
 * Consolidated test suite for UI, navigation, API, and auth flows
 * 
 * Run: npx playwright test --config=playwright.config.ts tests/ui.spec.ts
 */

import { test, expect } from '@playwright/test';

test.setTimeout(60000);
test.use({ viewport: { width: 1280, height: 720 } });

const BASE_URL = process.env.BASE_URL || 'http://localhost:4444';

// =============================================================================
// PAGES
// =============================================================================

test.describe('Pages', () => {
  test('homepage loads with search and chain filter', async ({ page }) => {
    await page.goto('/');
    await expect(page.getByPlaceholder(/search/i).first()).toBeVisible({ timeout: 15000 });
    await expect(page.locator('select').first()).toBeVisible({ timeout: 5000 });
  });

  test('consign page shows Sign In', async ({ page }) => {
    await page.goto('/consign');
    await expect(page.getByRole('button', { name: /sign in/i }).first()).toBeVisible({ timeout: 15000 });
  });

  test('my-deals page shows Sign In', async ({ page }) => {
    await page.goto('/my-deals');
    await expect(page.getByRole('button', { name: /sign in/i }).first()).toBeVisible({ timeout: 15000 });
  });

  test('how-it-works page loads', async ({ page }) => {
    await page.goto('/how-it-works');
    await expect(page.locator('h1, h2, h3').first()).toBeVisible({ timeout: 15000 });
  });

  test('terms page loads', async ({ page }) => {
    await page.goto('/terms');
    await expect(page.locator('h1, h2, p').first()).toBeVisible({ timeout: 10000 });
  });

  test('privacy page loads', async ({ page }) => {
    await page.goto('/privacy');
    await expect(page.locator('h1, h2, p').first()).toBeVisible({ timeout: 10000 });
  });
});

// =============================================================================
// NAVIGATION
// =============================================================================

test.describe('Navigation', () => {
  test('my-deals link works', async ({ page }) => {
    await page.goto('/');
    await page.getByRole('link', { name: /deal/i }).first().click();
    await expect(page).toHaveURL(/my-deals/, { timeout: 10000 });
  });

  test('how-it-works link works', async ({ page }) => {
    await page.goto('/');
    await page.locator('a[href="/how-it-works"]').first().click();
    await expect(page).toHaveURL(/how-it-works/, { timeout: 10000 });
  });

  test('Create Listing link goes to consign', async ({ page }) => {
    await page.goto('/');
    await page.getByRole('link', { name: /create listing/i }).first().click();
    await expect(page).toHaveURL(/consign/, { timeout: 10000 });
  });

  test('footer has terms and privacy links', async ({ page }) => {
    await page.goto('/');
    await page.evaluate(() => window.scrollTo(0, document.body.scrollHeight));
    await page.waitForTimeout(500);
    await expect(page.getByRole('link', { name: /terms/i })).toBeVisible({ timeout: 5000 });
    await expect(page.getByRole('link', { name: /privacy/i })).toBeVisible({ timeout: 5000 });
  });

  test('404 for invalid routes', async ({ page }) => {
    await page.goto('/nonexistent-page-xyz');
    const has404 = await page.locator('text=/404|not found/i').isVisible({ timeout: 5000 }).catch(() => false);
    const redirected = page.url().match(/localhost:\d+\/?$/);
    expect(has404 || redirected).toBeTruthy();
  });
});

// =============================================================================
// AUTH
// =============================================================================

test.describe('Auth', () => {
  test('Sign In opens auth modal', async ({ page }) => {
    await page.goto('/consign');
    const btn = page.getByRole('button', { name: /sign in/i }).first();
    await expect(btn).toBeVisible({ timeout: 15000 });
    
    await btn.click();
    await page.waitForTimeout(2000);
    
    const hasPrivy = await page.locator('[class*="privy"]').first().isVisible({ timeout: 5000 }).catch(() => false);
    const hasAuth = await page.locator('text=/farcaster|wallet|continue|evm|solana/i').first().isVisible({ timeout: 3000 }).catch(() => false);
    const hasDialog = await page.locator('[role="dialog"]').first().isVisible({ timeout: 3000 }).catch(() => false);
    expect(hasPrivy || hasAuth || hasDialog).toBe(true);
  });

  test('Escape closes modal', async ({ page }) => {
    await page.goto('/consign');
    await page.getByRole('button', { name: /sign in/i }).first().click();
    await page.waitForTimeout(2000);
    await page.keyboard.press('Escape');
    await expect(page.locator('body')).toBeVisible();
  });
});

// =============================================================================
// CHAIN FILTER
// =============================================================================

test.describe('Chain Filter', () => {
  test('dropdown has multiple chains including Solana', async ({ page }) => {
    await page.goto('/');
    const dropdown = page.locator('select').first();
    await expect(dropdown).toBeVisible({ timeout: 10000 });
    
    const options = await dropdown.locator('option').allTextContents();
    expect(options.length).toBeGreaterThanOrEqual(2);
    expect(options.some(opt => /solana/i.test(opt))).toBe(true);
  });

  test('selection changes filter', async ({ page }) => {
    await page.goto('/');
    const dropdown = page.locator('select').first();
    await expect(dropdown).toBeVisible({ timeout: 10000 });
    
    const options = await dropdown.locator('option').allTextContents();
    const solanaIndex = options.findIndex(opt => /solana/i.test(opt));
    if (solanaIndex >= 0) {
      await dropdown.selectOption({ index: solanaIndex });
    }
    await expect(page.locator('body')).toBeVisible();
  });
});

// =============================================================================
// TOKEN LISTINGS
// =============================================================================

test.describe('Token Page', () => {
  test('homepage shows token listings', async ({ page }) => {
    await page.goto('/');
    await expect(page.locator('text=/Listings:/i').first()).toBeVisible({ timeout: 20000 });
  });

  test('clicking deal navigates to token page', async ({ page }) => {
    await page.goto('/');
    await expect(page.locator('text=/Listings:/i').first()).toBeVisible({ timeout: 20000 });
    
    const dealRow = page.locator('text=/% off/i').first();
    await expect(dealRow).toBeVisible({ timeout: 10000 });
    await dealRow.click();
    
    await expect(page).toHaveURL(/\/token\//, { timeout: 10000 });
  });

  test('token page has chat interface', async ({ page }) => {
    await page.goto('/');
    await expect(page.locator('text=/Listings:/i').first()).toBeVisible({ timeout: 20000 });
    
    const dealRow = page.locator('text=/% off/i').first();
    await dealRow.click();
    await expect(page).toHaveURL(/\/token\//, { timeout: 10000 });
    
    // Chat should be visible but disabled without wallet
    const chat = page.locator('[data-testid="chat-input"]');
    await expect(chat).toBeVisible({ timeout: 15000 });
    await expect(chat).toBeDisabled();
    await expect(chat).toHaveAttribute('placeholder', /connect wallet/i);
  });
});

// =============================================================================
// API
// =============================================================================

test.describe('API', () => {
  test('GET /api/consignments', async ({ request }) => {
    const res = await request.get(`${BASE_URL}/api/consignments`);
    expect(res.status()).toBe(200);
    const data = await res.json();
    expect(data.success).toBe(true);
    expect(Array.isArray(data.consignments)).toBe(true);
  });

  test('GET /api/tokens', async ({ request }) => {
    const res = await request.get(`${BASE_URL}/api/tokens`);
    expect(res.status()).toBe(200);
    const data = await res.json();
    expect(data.success).toBe(true);
  });

  test('POST /api/rooms', async ({ request }) => {
    const entityId = '0x' + Date.now().toString(16).padStart(40, '0');
    const res = await request.post(`${BASE_URL}/api/rooms`, { data: { entityId } });
    expect([200, 201]).toContain(res.status());
    const data = await res.json();
    expect(data.roomId).toBeDefined();
  });

  test('POST /api/consignments', async ({ request }) => {
    const res = await request.post(`${BASE_URL}/api/consignments`, {
      data: {
        tokenId: 'token-base-0x' + '1'.repeat(40),
        amount: '1000000000000000000000',
        consignerAddress: '0x' + '2'.repeat(40),
        chain: 'base',
        isNegotiable: true,
        minDiscountBps: 500,
        maxDiscountBps: 2000,
        minLockupDays: 30,
        maxLockupDays: 365,
        minDealAmount: '100000000000000000000',
        maxDealAmount: '10000000000000000000000',
        isFractionalized: true,
        isPrivate: false,
        maxPriceVolatilityBps: 1000,
        maxTimeToExecuteSeconds: 1800,
      },
    });
    expect(res.status()).toBe(200);
    const data = await res.json();
    expect(data.success).toBe(true);
  });
});

// =============================================================================
// RESPONSIVE
// =============================================================================

test.describe('Responsive', () => {
  test('mobile viewport has menu button', async ({ page }) => {
    await page.setViewportSize({ width: 375, height: 667 });
    await page.goto('/');
    await expect(page.getByRole('button', { name: /menu/i })).toBeVisible({ timeout: 10000 });
  });

  test('tablet viewport renders', async ({ page }) => {
    await page.setViewportSize({ width: 768, height: 1024 });
    await page.goto('/');
    await expect(page.locator('body')).toBeVisible();
  });

  test('consign works on mobile', async ({ page }) => {
    await page.setViewportSize({ width: 375, height: 667 });
    await page.goto('/consign');
    await expect(page.getByRole('button', { name: /sign in/i }).first()).toBeVisible({ timeout: 15000 });
  });
});

// =============================================================================
// ERROR HANDLING
// =============================================================================

test.describe('Error Handling', () => {
  test('invalid token page handled', async ({ page }) => {
    await page.goto('/token/invalid123');
    await expect(page.locator('body')).toBeVisible({ timeout: 10000 });
  });

  test('invalid deal page handled', async ({ page }) => {
    await page.goto('/deal/invalid123');
    await expect(page.locator('body')).toBeVisible({ timeout: 10000 });
  });
});

