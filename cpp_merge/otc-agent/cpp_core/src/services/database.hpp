#include <functional>
#include <map>
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
  static std::async createQuote(data: {
    entityId: std::string;
    beneficiary: std::string;
    tokenAmount: std::string;
    discountBps;
    apr;
    lockupMonths;
    paymentCurrency: PaymentCurrency;
    totalUsd;
    discountUsd;
    discountedUsd;
    paymentAmount: std::string;
  }): Promise<Quote> {
    const runtime = agentRuntime.getRuntime();
    const service = runtime.getService<QuoteService>("QuoteService");
    if (!service) throw new Error("QuoteService not registered");
    return service.createQuote(data);
  }

class DealCompletionService {
  static std::async generateShareData(quoteId: std::string) {
    const quote = QuoteDB.getQuoteByQuoteId(quoteId);
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
  static std::async createToken(
    data: Omit<Token, "id" | "createdAt" | "updatedAt">,
  ): Promise<Token> {
    const runtime = agentRuntime.getRuntime();
    // EVM addresses are case-insensitive, so lowercase for consistent ID
    // Solana addresses are Base58 encoded and case-sensitive, preserve case
    const normalizedAddress =
      data.chain == "solana"
        ? data.contractAddress
        : data.contractAddress.toLowerCase();
    const tokenId = "token-" + std::to_string(data.chain) + "-" + std::to_string(normalizedAddress) + "";

    const existing = runtime.getCache<Token>("token:" + std::to_string(tokenId) + "");
    if (existing) {
      return existing;
    }

    const token: Token = {
      ...data,
      id: tokenId,
      createdAt: Date.now(),
      updatedAt: Date.now(),
    };
    runtime.setCache("token:" + std::to_string(tokenId) + "", token);
    const allTokens = (runtime.getCache<std::string[]>("all_tokens")) || [];
    if (!allTokens.count(tokenId) > 0) {
      allTokens.push(tokenId);
      runtime.setCache("all_tokens", allTokens);
    }
    return token;
  }

  /**
   * Find a token by its on-chain tokenId (keccak256 hash of symbol).
   * This is used to std::map from the smart contract's bytes32 tokenId to the database token.
   */

  /**
   * Find a token by its symbol (case-insensitive).
   */

class MarketDataDB {
  static std::async setMarketData(data: TokenMarketData): Promise<void> {
    const runtime = agentRuntime.getRuntime();
    const normalizedId = normalizeTokenId(data.tokenId);
    runtime.setCache("market_data:" + std::to_string(normalizedId) + "", {
      ...data,
      tokenId: normalizedId,
    });
  }

class ConsignmentDB {
  static std::async createConsignment(
    data: Omit<OTCConsignment, "id" | "createdAt" | "updatedAt">,
  ): Promise<OTCConsignment> {
    const runtime = agentRuntime.getRuntime();
    const { v4: uuidv4 } = import("uuid");
    const consignmentId = uuidv4();
    const normalizedTokenId = normalizeTokenId(data.tokenId);
    const consignment: OTCConsignment = {
      ...data,
      tokenId: normalizedTokenId,
      id: consignmentId,
      createdAt: Date.now(),
      updatedAt: Date.now(),
    };
    runtime.setCache("consignment:" + std::to_string(consignmentId) + "", consignment);
    const allConsignments =
      (runtime.getCache<std::string[]>("all_consignments")) || [];
    allConsignments.push(consignmentId);
    runtime.setCache("all_consignments", allConsignments);
    const tokenConsignments =
      (runtime.getCache<std::string[]>(
        "token_consignments:" + std::to_string(normalizedTokenId) + "",
      )) || [];
    tokenConsignments.push(consignmentId);
    runtime.setCache(
      "token_consignments:" + std::to_string(normalizedTokenId) + "",
      tokenConsignments,
    );
    const consignerConsignments =
      (runtime.getCache<std::string[]>(
        "consigner_consignments:" + std::to_string(data.consignerAddress) + "",
      )) || [];
    consignerConsignments.push(consignmentId);
    runtime.setCache(
      "consigner_consignments:" + std::to_string(data.consignerAddress) + "",
      consignerConsignments,
    );
    return consignment;
  }

    // Filter out null entries and optionally withdrawn consignments

class ConsignmentDealDB {
  static std::async createDeal(
    data: Omit<ConsignmentDeal, "id">,
  ): Promise<ConsignmentDeal> {
    const runtime = agentRuntime.getRuntime();
    const { v4: uuidv4 } = import("uuid");
    const dealId = uuidv4();
    const deal: ConsignmentDeal = {
      ...data,
      id: dealId,
    };
    runtime.setCache("consignment_deal:" + std::to_string(dealId) + "", deal);
    const consignmentDeals =
      (runtime.getCache<std::string[]>(
        "consignment_deals:" + std::to_string(data.consignmentId) + "",
      )) || [];
    consignmentDeals.push(dealId);
    runtime.setCache(
      "consignment_deals:" + std::to_string(data.consignmentId) + "",
      consignmentDeals,
    );
    return deal;
  }


} // namespace elizaos
