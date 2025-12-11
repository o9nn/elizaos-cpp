// Shared types for OTC Desk plugin - export for external use only

export type QuoteStatus =
  | "active"
  | "expired"
  | "executed"
  | "rejected"
  | "approved";
export type PaymentCurrency = "ETH" | "USDC";
export type ChainType = "evm" | "solana";

export interface QuoteMemory {
  id: string;
  quoteId: string;
  entityId: string;
  beneficiary: string;
  tokenAmount: string;
  discountBps: number;
  apr: number;
  lockupMonths: number;
  lockupDays: number;
  paymentCurrency: PaymentCurrency;
  priceUsdPerToken: number;
  totalUsd: number;
  discountUsd: number;
  discountedUsd: number;
  paymentAmount: string;
  status: QuoteStatus;
  signature: string;
  createdAt: number;
  executedAt: number;
  rejectedAt: number;
  approvedAt: number;
  offerId: string;
  transactionHash: string;
  blockNumber: number;
  rejectionReason: string;
  approvalNote: string;
  // Optional chain context to distinguish EVM vs Solana flows
  chain?: ChainType;
  // Token metadata for display
  tokenId?: string;
  tokenSymbol?: string;
  tokenName?: string;
  tokenLogoUrl?: string;
  // Consignment reference
  consignmentId?: string;
}

export interface UserSessionMemory {
  id: string;
  entityId: string;
  walletAddress: string;
  quotesCreated: number;
  lastQuoteAt: number;
  dailyQuoteCount: number;
  dailyResetAt: number;
  totalDeals: number;
  totalVolumeUsd: number;
  totalSavedUsd: number;
  createdAt: number;
  updatedAt: number;
}
