import { defineWalletSetup } from '@synthetixio/synpress';
import { MetaMask } from '@synthetixio/synpress/playwright';

/**
 * Seller/Consigner wallet setup for two-party OTC testing
 * This wallet will create consignments and receive payments
 */

// Seller uses a different derivation path or a specific key
// For Anvil, account #0 is: 0xf39Fd6e51aad88F6F4ce6aB8827279cffFb92266
const SELLER_SEED = process.env.SELLER_SEED_PHRASE || 'test test test test test test test test test test test junk';
const PASSWORD = process.env.WALLET_PASSWORD || 'Tester@1234';

export default defineWalletSetup(PASSWORD, async (context, walletPage) => {
  // Wait for page to be fully loaded in CI
  await walletPage.waitForLoadState('domcontentloaded');
  
  const metamask = new MetaMask(context, walletPage, PASSWORD);
  await metamask.importWallet(SELLER_SEED);

  // Add Anvil network
  const chainId = parseInt(process.env.CHAIN_ID || '31337'); // Anvil default
  const rpcUrl = process.env.NEXT_PUBLIC_RPC_URL || 'http://localhost:8545';

  try {
    await metamask.addNetwork({
      name: 'Anvil Localnet',
      rpcUrl: rpcUrl,
      chainId: chainId,
      symbol: 'ETH',
    });
  } catch (e) {
    console.log('Network may already be added, continuing...', e);
  }
});

export const sellerSetup = {
  walletPassword: PASSWORD,
  seedPhrase: SELLER_SEED,
};

