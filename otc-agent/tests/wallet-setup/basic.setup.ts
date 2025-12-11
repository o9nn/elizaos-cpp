import { defineWalletSetup } from '@synthetixio/synpress';
import { MetaMask } from '@synthetixio/synpress/playwright';

/**
 * Basic MetaMask wallet setup for Synpress tests
 * 
 * Uses the standard Anvil test seed phrase which gives us:
 * - Account 0: 0xf39Fd6e51aad88F6F4ce6aB8827279cffFb92266 (10000 ETH)
 * - Account 1: 0x70997970C51812dc3A010C7d01b50e0d17dc79C8 (10000 ETH)
 * etc.
 * 
 * Chain configuration:
 * - Anvil: chainId 31337 at http://localhost:8545
 * 
 * IMPORTANT: This setup is cached by Synpress. The wallet state is stored in
 * .cache/synpress-cache and reused across test runs to prevent re-setup loops.
 */

const SEED_PHRASE = process.env.SEED_PHRASE || 'test test test test test test test test test test test junk';
const PASSWORD = process.env.WALLET_PASSWORD || 'Tester@1234';

// Wallet setup function - this runs ONCE and is cached
const setupWallet = defineWalletSetup(PASSWORD, async (context, walletPage) => {
  // Wait for page to be fully loaded in CI
  await walletPage.waitForLoadState('domcontentloaded');
  
  const metamask = new MetaMask(context, walletPage, PASSWORD);
  
  // Import wallet with test seed
  await metamask.importWallet(SEED_PHRASE);

  // Add Anvil network - chainId 31337
  const chainId = parseInt(process.env.CHAIN_ID || '31337');
  const rpcUrl = process.env.NEXT_PUBLIC_RPC_URL || 'http://localhost:8545';
  
  try {
    await metamask.addNetwork({
      name: 'Anvil Localnet',
      rpcUrl: rpcUrl,
      chainId: chainId,
      symbol: 'ETH',
    });
  } catch (e) {
    // Network may already exist - that's fine
    console.log('Network may already be added, continuing...', e);
  }

  // Switch to Anvil network
  try {
    await metamask.switchNetwork('Anvil Localnet');
  } catch (e) {
    // Already on the network or network doesn't exist yet
    console.log('Could not switch network, continuing...', e);
  }
});

// Export password for tests to use with MetaMask class
export const walletPassword = PASSWORD;
export const seedPhrase = SEED_PHRASE;

// Default export is the wallet setup
export default setupWallet;

