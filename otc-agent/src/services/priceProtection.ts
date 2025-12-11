import { MarketDataDB } from "./database";
import type { Chain } from "@/config/chains";
import { MarketDataService } from "./marketDataService";

export interface ValidationResult {
  isValid: boolean;
  currentPrice: number;
  priceAtQuote: number;
  deviation: number;
  deviationBps: number;
  maxAllowedDeviationBps: number;
  reason?: string;
}

export class PriceProtectionService {
  private marketDataService: MarketDataService;

  constructor() {
    this.marketDataService = new MarketDataService();
  }

  async validateQuotePrice(
    tokenId: string,
    tokenAddress: string,
    chain: Chain,
    priceAtQuote: number,
    maxDeviationBps: number,
  ): Promise<ValidationResult> {
    const marketData = await MarketDataDB.getMarketData(tokenId);

    let currentPrice: number;
    if (!marketData || Date.now() - marketData.lastUpdated > 300000) {
      currentPrice = await this.marketDataService.fetchTokenPrice(
        tokenAddress,
        chain,
      );
    } else {
      currentPrice = marketData.priceUsd;
    }

    const deviation = Math.abs(currentPrice - priceAtQuote);
    const deviationBps = Math.floor((deviation / priceAtQuote) * 10000);

    const isValid = deviationBps <= maxDeviationBps;

    return {
      isValid,
      currentPrice,
      priceAtQuote,
      deviation,
      deviationBps,
      maxAllowedDeviationBps: maxDeviationBps,
      reason: isValid
        ? undefined
        : `Price moved ${deviationBps / 100}%, exceeding maximum allowed ${maxDeviationBps / 100}%`,
    };
  }
}
