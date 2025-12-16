#include "..entityId.hpp"
#include "elizaos/core.hpp"
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

// Quote Service - quote management for Eliza OTC Desk
// Single source of truth registered with runtime.getService("QuoteService")

import type { IAgentRuntime } from "@elizaos/core";
;
;
;
;
import type { QuoteMemory, PaymentCurrency, QuoteStatus } from "../types";

const QUOTE_KEY = (quoteId: string) => `quote:${quoteId}`;
const ENTITY_QUOTES_KEY = (entityId: string) => `entity_quotes:${entityId}`;
const BENEFICIARY_QUOTES_KEY = (beneficiary: string) =>
  `beneficiary_quotes:${beneficiary.toLowerCase()}`;
const ALL_QUOTES_KEY = "all_quotes";

class QuoteService extends Service {
  static serviceType = "QuoteService" as any;
  static serviceName = "QuoteService";

  get serviceType(): string {
    return "QuoteService";
  }

  capabilityDescription = "QuoteService";

  constructor(runtime: IAgentRuntime) {
    super();
    this.runtime = runtime;
  }

  async initialize(): Promise<void> {
    console.log(
      "[QuoteService] Initialized - single source of truth for quotes",
    );
    console.log("[QuoteService] Service type:", this.serviceType);
    console.log("[QuoteService] Service name:", QuoteService.serviceName);
  }

  async stop(): Promise<void> {
    // Cleanup if needed
  }

  static async start(runtime: IAgentRuntime): Promise<QuoteService> {
    const service = new QuoteService(runtime);
    await service.initialize();
    return service;
  }

  private async addToIndex(
    quoteId: string,
    entityId: string,
    beneficiary?: string,
  ): Promise<void> {
    const allQuotes =
      (await this.runtime.getCache<string[]>(ALL_QUOTES_KEY)) ?? [];
    if (!allQuotes.includes(quoteId)) {
      allQuotes.push(quoteId);
      await this.runtime.setCache(ALL_QUOTES_KEY, allQuotes);
    }

    const entityQuotes =
      (await this.runtime.getCache<string[]>(ENTITY_QUOTES_KEY(entityId))) ??
      [];
    if (!entityQuotes.includes(quoteId)) {
      entityQuotes.push(quoteId);
      await this.runtime.setCache(ENTITY_QUOTES_KEY(entityId), entityQuotes);
    }

    // Also index by beneficiary for faster lookups
    if (beneficiary) {
      const beneficiaryQuotes =
        (await this.runtime.getCache<string[]>(
          BENEFICIARY_QUOTES_KEY(beneficiary),
        )) ?? [];
      if (!beneficiaryQuotes.includes(quoteId)) {
        beneficiaryQuotes.push(quoteId);
        await this.runtime.setCache(
          BENEFICIARY_QUOTES_KEY(beneficiary),
          beneficiaryQuotes,
        );
      }
    }
  }

  private generateQuoteId(entityId: string): string {
    // Generate deterministic quote ID from entityId
    // Use a hash of entityId + current day to allow one quote per wallet per day
    const dayTimestamp = Math.floor(Date.now() / (24 * 60 * 60 * 1000));
    const hash = crypto
      .createHash("sha256")
      .update(`${entityId}-${dayTimestamp}`)
      .digest("hex")
      .substring(0, 12)
      .toUpperCase();
    return `OTC-${hash}`;
  }

  private generateQuoteSignature(data: {
    quoteId: string;
    entityId: string;
    beneficiary: string;
    tokenAmount: string;
    discountBps: number;
    lockupMonths: number;
  }): string {
    const secret = process.env.WORKER_AUTH_TOKEN;
    if (!secret) {
      throw new Error("WORKER_AUTH_TOKEN must be set for quote signature generation");
    }
    const payload = JSON.stringify(data);
    return crypto.createHmac("sha256", secret).update(payload).digest("hex");
  }

  async createQuote(data: {
    entityId: string;
    beneficiary: string;
    tokenAmount: string;
    discountBps: number;
    apr: number;
    lockupMonths: number;
    paymentCurrency: PaymentCurrency;
    priceUsdPerToken?: number;
    totalUsd: number;
    discountUsd: number;
    discountedUsd: number;
    paymentAmount: string;
    // Token metadata (optional but recommended)
    tokenId?: string;
    tokenSymbol?: string;
    tokenName?: string;
    tokenLogoUrl?: string;
    chain?: "evm" | "solana";
    consignmentId?: string;
  }): Promise<QuoteMemory> {
    const quoteId = this.generateQuoteId(data.entityId);
    const lockupDays = data.lockupMonths * 30;
    const now = Date.now();

    // Check if quote already exists
    const existing = await this.runtime.getCache<QuoteMemory>(
      QUOTE_KEY(quoteId),
    );
    console.log(
      `[QuoteService] createQuote - ID: ${quoteId}, Exists: ${!!existing}, Terms: ${data.discountBps}bps/${data.lockupMonths}mo`,
    );

    const signature = this.generateQuoteSignature({
      quoteId,
      entityId: data.entityId,
      beneficiary: data.beneficiary,
      tokenAmount: data.tokenAmount,
      discountBps: data.discountBps,
      lockupMonths: data.lockupMonths,
    });

    const normalizedBeneficiary = data.beneficiary.toLowerCase();

    const quoteData: QuoteMemory = {
      id: existing?.id || uuidv4(), // Keep same internal ID if updating
      quoteId,
      entityId: data.entityId,
      beneficiary: normalizedBeneficiary,
      tokenAmount: data.tokenAmount,
      discountBps: data.discountBps,
      apr: data.apr,
      lockupMonths: data.lockupMonths,
      lockupDays,
      paymentCurrency: data.paymentCurrency,
      priceUsdPerToken: data.priceUsdPerToken || 0,
      totalUsd: data.totalUsd,
      discountUsd: data.discountUsd,
      discountedUsd: data.discountedUsd,
      paymentAmount: data.paymentAmount,
      signature,
      status: "active",
      createdAt: existing?.createdAt || now, // Keep original creation time if updating
      executedAt: 0,
      rejectedAt: 0,
      approvedAt: 0,
      offerId: "",
      transactionHash: "",
      blockNumber: 0,
      rejectionReason: "",
      approvalNote: "",
      // Token metadata
      tokenId: data.tokenId,
      tokenSymbol: data.tokenSymbol,
      tokenName: data.tokenName,
      tokenLogoUrl: data.tokenLogoUrl,
      chain: data.chain,
      consignmentId: data.consignmentId,
    };

    await this.runtime.setCache(QUOTE_KEY(quoteId), quoteData);
    await this.addToIndex(quoteId, data.entityId, normalizedBeneficiary);

    console.log(
      `[QuoteService] ✅ Quote stored: ${quoteId} - ${data.discountBps}bps/${data.lockupMonths}mo - token: ${data.tokenSymbol || "unknown"}`,
    );
    return quoteData;
  }

  async getActiveQuotes(): Promise<QuoteMemory[]> {
    const allQuoteIds =
      (await this.runtime.getCache<string[]>(ALL_QUOTES_KEY)) ?? [];

    // Fetch all quotes in parallel for better performance
    const allQuotes = await Promise.all(
      allQuoteIds.map((quoteId) =>
        this.runtime.getCache<QuoteMemory>(QUOTE_KEY(quoteId)),
      ),
    );

    return allQuotes
      .filter(
        (quote): quote is QuoteMemory =>
          quote != null && quote.status === "active",
      )
      .sort((a, b) => b.createdAt - a.createdAt);
  }

  async getQuoteByBeneficiary(beneficiary: string): Promise<QuoteMemory> {
    const normalized = beneficiary.toLowerCase();

    // First try the beneficiary index (fast path)
    const beneficiaryQuoteIds = await this.runtime.getCache<string[]>(
      BENEFICIARY_QUOTES_KEY(normalized),
    );

    if (beneficiaryQuoteIds && beneficiaryQuoteIds.length > 0) {
      // Fetch indexed quotes in parallel
      const indexedQuotes = await Promise.all(
        beneficiaryQuoteIds.map((quoteId) =>
          this.runtime.getCache<QuoteMemory>(QUOTE_KEY(quoteId)),
        ),
      );

      const activeQuote = indexedQuotes.find(
        (quote) => quote != null && quote.status === "active",
      );
      if (activeQuote) return activeQuote;
    }

    // Fallback: search all quotes in parallel (slower but complete)
    const allQuoteIds =
      (await this.runtime.getCache<string[]>(ALL_QUOTES_KEY)) ?? [];

    const allQuotes = await Promise.all(
      allQuoteIds.map((quoteId) =>
        this.runtime.getCache<QuoteMemory>(QUOTE_KEY(quoteId)),
      ),
    );

    const found = allQuotes.find(
      (quote) =>
        quote != null &&
        quote.beneficiary === normalized &&
        quote.status === "active",
    );

    if (found) return found;
    throw new Error(`No active quote found for beneficiary: ${beneficiary}`);
  }

  async getUserQuoteHistory(
    entityId: string,
    limit: number,
  ): Promise<QuoteMemory[]> {
    console.log("[QuoteService] getUserQuoteHistory:", { entityId, limit });

    const entityQuoteIds =
      (await this.runtime.getCache<string[]>(ENTITY_QUOTES_KEY(entityId))) ??
      [];
    console.log(
      "[QuoteService] Found quote IDs from index:",
      entityQuoteIds.length,
    );

    // Fetch entity quotes in parallel
    const entityQuotes = await Promise.all(
      entityQuoteIds.map((quoteId) =>
        this.runtime.getCache<QuoteMemory>(QUOTE_KEY(quoteId)),
      ),
    );

    const quotes = entityQuotes.filter((q): q is QuoteMemory => q != null);
    const processedIds = new Set(entityQuoteIds);

    // Search for unindexed quotes (in parallel)
    const allQuoteIds =
      (await this.runtime.getCache<string[]>(ALL_QUOTES_KEY)) ?? [];
    const uncheckedIds = allQuoteIds.filter((id) => !processedIds.has(id));

    if (uncheckedIds.length > 0) {
      const uncheckedQuotes = await Promise.all(
        uncheckedIds.map((quoteId) =>
          this.runtime.getCache<QuoteMemory>(QUOTE_KEY(quoteId)),
        ),
      );

      // Find quotes with matching entityId that weren't indexed
      const unindexedQuoteIds: string[] = [];
      for (let i = 0; i < uncheckedQuotes.length; i++) {
        const quote = uncheckedQuotes[i];
        if (quote && quote.entityId === entityId) {
          quotes.push(quote);
          unindexedQuoteIds.push(uncheckedIds[i]);
        }
      }

      // Fix the index in batch if we found any
      if (unindexedQuoteIds.length > 0) {
        console.log(
          "[QuoteService] Found unindexed quotes:",
          unindexedQuoteIds.length,
        );
        const updatedEntityQuoteIds = [...entityQuoteIds, ...unindexedQuoteIds];
        await this.runtime.setCache(
          ENTITY_QUOTES_KEY(entityId),
          updatedEntityQuoteIds,
        );
      }
    }

    const sorted = quotes
      .sort((a, b) => b.createdAt - a.createdAt)
      .slice(0, limit);
    console.log("[QuoteService] Returning quotes:", sorted.length);
    return sorted;
  }

  async getQuoteByQuoteId(quoteId: string): Promise<QuoteMemory> {
    const quote = await this.runtime.getCache<QuoteMemory>(QUOTE_KEY(quoteId));
    if (!quote) throw new Error(`Quote not found: ${quoteId}`);
    return quote;
  }

  async getQuoteByOfferId(offerId: string): Promise<QuoteMemory | null> {
    const allQuoteIds =
      (await this.runtime.getCache<string[]>(ALL_QUOTES_KEY)) ?? [];

    // Fetch all quotes in parallel
    const allQuotes = await Promise.all(
      allQuoteIds.map((quoteId) =>
        this.runtime.getCache<QuoteMemory>(QUOTE_KEY(quoteId)),
      ),
    );

    return (
      allQuotes.find((quote) => quote != null && quote.offerId === offerId) ??
      null
    );
  }

  async updateQuoteStatus(
    quoteId: string,
    status: QuoteStatus,
    data: {
      offerId: string;
      transactionHash: string;
      blockNumber: number;
      rejectionReason: string;
      approvalNote: string;
    },
  ): Promise<QuoteMemory> {
    const quote = await this.getQuoteByQuoteId(quoteId);
    const now = Date.now();

    const updatedQuote: QuoteMemory = {
      ...quote,
      status,
      offerId: data.offerId || quote.offerId,
      transactionHash: data.transactionHash || quote.transactionHash,
      blockNumber: data.blockNumber || quote.blockNumber,
      rejectionReason: data.rejectionReason || quote.rejectionReason,
      approvalNote: data.approvalNote || quote.approvalNote,
      executedAt: status === "executed" ? now : quote.executedAt,
      rejectedAt: status === "rejected" ? now : quote.rejectedAt,
      approvedAt: status === "approved" ? now : quote.approvedAt,
    };

    await this.runtime.setCache(QUOTE_KEY(quoteId), updatedQuote);
    return updatedQuote;
  }

  async updateQuoteExecution(
    quoteId: string,
    data: {
      tokenAmount: string;
      totalUsd: number;
      discountUsd: number;
      discountedUsd: number;
      paymentCurrency: PaymentCurrency;
      paymentAmount: string;
      offerId: string;
      transactionHash: string;
      blockNumber: number;
      priceUsdPerToken?: number;
      ethUsdPrice?: number;
      lockupDays?: number;
    },
  ): Promise<QuoteMemory> {
    console.log("[QuoteService] updateQuoteExecution called:", {
      quoteId,
      offerId: data.offerId,
      tokenAmount: data.tokenAmount,
      priceUsdPerToken: data.priceUsdPerToken,
    });

    const quote = await this.getQuoteByQuoteId(quoteId);
    console.log("[QuoteService] Current quote status:", quote.status);

    const updatedQuote: QuoteMemory = {
      ...quote,
      tokenAmount: data.tokenAmount,
      totalUsd: data.totalUsd,
      discountUsd: data.discountUsd,
      discountedUsd: data.discountedUsd,
      paymentCurrency: data.paymentCurrency,
      paymentAmount: data.paymentAmount,
      offerId: data.offerId,
      transactionHash: data.transactionHash,
      blockNumber: data.blockNumber,
      priceUsdPerToken: data.priceUsdPerToken ?? quote.priceUsdPerToken,
      lockupDays: data.lockupDays ?? quote.lockupDays,
      status: "executed",
      executedAt: Date.now(),
    };

    await this.runtime.setCache(QUOTE_KEY(quoteId), updatedQuote);
    console.log("[QuoteService] Quote status updated to EXECUTED:", {
      quoteId: updatedQuote.quoteId,
      status: updatedQuote.status,
      entityId: updatedQuote.entityId,
    });

    return updatedQuote;
  }

  async setQuoteBeneficiary(
    quoteId: string,
    beneficiary: string,
  ): Promise<QuoteMemory> {
    console.log("[QuoteService] setQuoteBeneficiary called:", {
      quoteId,
      beneficiary,
    });

    const quote = await this.getQuoteByQuoteId(quoteId);
    const normalized = beneficiary.toLowerCase();
    const newEntityId = walletToEntityId(normalized);

    console.log("[QuoteService] Updating quote beneficiary:", {
      quoteId,
      oldBeneficiary: quote.beneficiary,
      newBeneficiary: normalized,
      oldEntityId: quote.entityId,
      newEntityId,
    });

    // Remove from old entity's quote list
    const oldEntityQuotes =
      (await this.runtime.getCache<string[]>(
        ENTITY_QUOTES_KEY(quote.entityId),
      )) || [];
    console.log(
      "[QuoteService] Old entity quotes:",
      oldEntityQuotes.length,
      oldEntityQuotes,
    );
    const filteredOld = oldEntityQuotes.filter((id) => id !== quoteId);
    await this.runtime.setCache(ENTITY_QUOTES_KEY(quote.entityId), filteredOld);
    console.log(
      "[QuoteService] Removed from old entity, remaining:",
      filteredOld.length,
    );

    // Remove from old beneficiary's index
    if (quote.beneficiary) {
      const oldBeneficiaryQuotes =
        (await this.runtime.getCache<string[]>(
          BENEFICIARY_QUOTES_KEY(quote.beneficiary),
        )) || [];
      const filteredOldBeneficiary = oldBeneficiaryQuotes.filter(
        (id) => id !== quoteId,
      );
      await this.runtime.setCache(
        BENEFICIARY_QUOTES_KEY(quote.beneficiary),
        filteredOldBeneficiary,
      );
    }

    // Add to new entity's quote list
    const newEntityQuotes =
      (await this.runtime.getCache<string[]>(ENTITY_QUOTES_KEY(newEntityId))) ||
      [];
    console.log(
      "[QuoteService] New entity quotes before:",
      newEntityQuotes.length,
      newEntityQuotes,
    );
    if (!newEntityQuotes.includes(quoteId)) {
      newEntityQuotes.push(quoteId);
      await this.runtime.setCache(
        ENTITY_QUOTES_KEY(newEntityId),
        newEntityQuotes,
      );
      console.log(
        "[QuoteService] Added to new entity, total:",
        newEntityQuotes.length,
      );
    }

    // Add to new beneficiary's index
    const newBeneficiaryQuotes =
      (await this.runtime.getCache<string[]>(
        BENEFICIARY_QUOTES_KEY(normalized),
      )) || [];
    if (!newBeneficiaryQuotes.includes(quoteId)) {
      newBeneficiaryQuotes.push(quoteId);
      await this.runtime.setCache(
        BENEFICIARY_QUOTES_KEY(normalized),
        newBeneficiaryQuotes,
      );
    }

    const newSignature = this.generateQuoteSignature({
      quoteId: quote.quoteId,
      entityId: newEntityId,
      beneficiary: normalized,
      tokenAmount: quote.tokenAmount,
      discountBps: quote.discountBps,
      lockupMonths: quote.lockupMonths,
    });

    const updatedQuote: QuoteMemory = {
      ...quote,
      beneficiary: normalized,
      entityId: newEntityId,
      signature: newSignature,
    };

    await this.runtime.setCache(QUOTE_KEY(quoteId), updatedQuote);
    console.log("[QuoteService] Quote updated and cached:", {
      quoteId: updatedQuote.quoteId,
      entityId: updatedQuote.entityId,
      beneficiary: updatedQuote.beneficiary,
    });

    return updatedQuote;
  }

  verifyQuoteSignature(quote: QuoteMemory): boolean {
    const expectedSignature = this.generateQuoteSignature({
      quoteId: quote.quoteId,
      entityId: quote.entityId,
      beneficiary: quote.beneficiary,
      tokenAmount: quote.tokenAmount,
      discountBps: quote.discountBps,
      lockupMonths: quote.lockupMonths,
    });

    return quote.signature === expectedSignature;
  }

  // Helper: Get latest active quote by wallet address
  async getQuoteByWallet(
    walletAddress: string,
  ): Promise<QuoteMemory | undefined> {
    const entityId = walletToEntityId(walletAddress);
    const quotes = await this.getUserQuoteHistory(entityId, 100);
    // Return the most recent ACTIVE quote only
    return quotes.find((q) => q.entityId === entityId && q.status === "active");
  }

  // Helper: Expire all active quotes for a user (called before creating new one)
  async expireUserQuotes(walletAddress: string): Promise<void> {
    const entityId = walletToEntityId(walletAddress);
    const active = await this.getActiveQuotes();
    const userQuotes = active.filter((q) => q.entityId === entityId);

    console.log(
      `[QuoteService] Expiring ${userQuotes.length} quotes for ${walletAddress}`,
    );

    // Expire all quotes in parallel for better performance
    await Promise.all(
      userQuotes.map(async (quote) => {
        console.log(
          `[QuoteService] Expiring quote: ${quote.quoteId} (${quote.discountBps}bps/${quote.lockupMonths}mo)`,
        );
        return this.updateQuoteStatus(quote.quoteId, "expired", {
          offerId: "",
          transactionHash: "",
          blockNumber: 0,
          rejectionReason: "Replaced by new quote",
          approvalNote: "",
        });
      }),
    );
  }
}

// Helper to get service from runtime


default QuoteService;

} // namespace elizaos
