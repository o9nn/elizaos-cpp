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

// Database service layer using Eliza runtime services

class QuoteDB {
  static async createQuote(data: {
    entityId: string;
    beneficiary: string;
    tokenAmount: string;
    discountBps: number;
    apr: number;
    lockupMonths: number;
    paymentCurrency: PaymentCurrency;
    totalUsd: number;
    discountUsd: number;
    discountedUsd: number;
    paymentAmount: string;
  }): Promise<Quote> {
    const runtime = await agentRuntime.getRuntime();
    const service = runtime.getService<QuoteService>("QuoteService");
    if (!service) throw new Error("QuoteService not registered");
    return await service.createQuote(data);
  }

class DealCompletionService {
  static async generateShareData(quoteId: string) {
    const quote = await QuoteDB.getQuoteByQuoteId(quoteId);
    return {
      quote,
    };
  }

/**
 * Normalizes a tokenId to ensure consistent lookups.
 * EVM addresses are case-insensitive, so they are lowercased.
 * Solana addresses (Base58) are case-sensitive, so they are preserved.
 * Format: token-{chain}-{address}
 */
std::string normalizeTokenId(const std::string& tokenId);

class TokenDB {
  static async createToken(
    data: Omit<Token, "id" | "createdAt" | "updatedAt">,
  ): Promise<Token> {
    const runtime = await agentRuntime.getRuntime();
    // EVM addresses are case-insensitive, so lowercase for consistent ID
    // Solana addresses are Base58 encoded and case-sensitive, preserve case
    const normalizedAddress =
      data.chain === "solana"
        ? data.contractAddress
        : data.contractAddress.toLowerCase();
    const tokenId = `token-${data.chain}-${normalizedAddress}`;

    const existing = await runtime.getCache<Token>(`token:${tokenId}`);
    if (existing) {
      return existing;
    }

    const token: Token = {
      ...data,
      id: tokenId,
      createdAt: Date.now(),
      updatedAt: Date.now(),
    };
    await runtime.setCache(`token:${tokenId}`, token);
    const allTokens = (await runtime.getCache<string[]>("all_tokens")) ?? [];
    if (!allTokens.includes(tokenId)) {
      allTokens.push(tokenId);
      await runtime.setCache("all_tokens", allTokens);
    }
    return token;
  }

  /**
   * Find a token by its on-chain tokenId (keccak256 hash of symbol).
   * This is used to map from the smart contract's bytes32 tokenId to the database token.
   */

  /**
   * Find a token by its symbol (case-insensitive).
   */

class MarketDataDB {
  static async setMarketData(data: TokenMarketData): Promise<void> {
    const runtime = await agentRuntime.getRuntime();
    const normalizedId = normalizeTokenId(data.tokenId);
    await runtime.setCache(`market_data:${normalizedId}`, {
      ...data,
      tokenId: normalizedId,
    });
  }

class ConsignmentDB {
  static async createConsignment(
    data: Omit<OTCConsignment, "id" | "createdAt" | "updatedAt">,
  ): Promise<OTCConsignment> {
    const runtime = await agentRuntime.getRuntime();
    const { v4: uuidv4 } = await import("uuid");
    const consignmentId = uuidv4();
    const normalizedTokenId = normalizeTokenId(data.tokenId);
    const consignment: OTCConsignment = {
      ...data,
      tokenId: normalizedTokenId,
      id: consignmentId,
      createdAt: Date.now(),
      updatedAt: Date.now(),
    };
    await runtime.setCache(`consignment:${consignmentId}`, consignment);
    const allConsignments =
      (await runtime.getCache<string[]>("all_consignments")) ?? [];
    allConsignments.push(consignmentId);
    await runtime.setCache("all_consignments", allConsignments);
    const tokenConsignments =
      (await runtime.getCache<string[]>(
        `token_consignments:${normalizedTokenId}`,
      )) ?? [];
    tokenConsignments.push(consignmentId);
    await runtime.setCache(
      `token_consignments:${normalizedTokenId}`,
      tokenConsignments,
    );
    const consignerConsignments =
      (await runtime.getCache<string[]>(
        `consigner_consignments:${data.consignerAddress}`,
      )) ?? [];
    consignerConsignments.push(consignmentId);
    await runtime.setCache(
      `consigner_consignments:${data.consignerAddress}`,
      consignerConsignments,
    );
    return consignment;
  }

    // Filter out null entries and optionally withdrawn consignments

class ConsignmentDealDB {
  static async createDeal(
    data: Omit<ConsignmentDeal, "id">,
  ): Promise<ConsignmentDeal> {
    const runtime = await agentRuntime.getRuntime();
    const { v4: uuidv4 } = await import("uuid");
    const dealId = uuidv4();
    const deal: ConsignmentDeal = {
      ...data,
      id: dealId,
    };
    await runtime.setCache(`consignment_deal:${dealId}`, deal);
    const consignmentDeals =
      (await runtime.getCache<string[]>(
        `consignment_deals:${data.consignmentId}`,
      )) ?? [];
    consignmentDeals.push(dealId);
    await runtime.setCache(
      `consignment_deals:${data.consignmentId}`,
      consignmentDeals,
    );
    return deal;
  }


} // namespace elizaos
