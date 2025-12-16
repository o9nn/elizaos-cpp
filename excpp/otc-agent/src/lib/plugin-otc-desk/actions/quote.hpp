#include ".providers/quote.hpp"
#include ".services/priceFeed.hpp"
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

// quote action - generate a new OTC quote and return an XML object to the frontend

;
;
;
;
;

 {
  const result: any = {};

  // Parse token amount (various formats)
  const amountMatch = text.match(
    /(\d+(?:,\d{3})*(?:\.\d+)?)\s*(?:tokens?|eliza)?/i,
  );
  if (amountMatch) {
    result.tokenAmount = amountMatch[1].replace(/,/g, "");
  }

  // Parse discount (percentage or bps)
  const discountMatch = text.match(
    /(\d+(?:\.\d+)?)\s*(?:%|percent|bps|basis)/i,
  );
  if (discountMatch) {
    const value = parseFloat(discountMatch[1]);
    // If it's a percentage (has % or "percent"), convert to bps
    if (
      discountMatch[0].includes("%") ||
      discountMatch[0].includes("percent")
    ) {
      result.discountBps = Math.round(value * 100);
    } else {
      result.discountBps = Math.round(value);
    }
  }

  // Parse payment currency
  if (/(\b(eth|ethereum)\b)/i.test(text)) {
    result.paymentCurrency = "ETH";
  } else if (/(\b(usdc|usd|dollar)\b)/i.test(text)) {
    result.paymentCurrency = "USDC";
  }

  return result;
}

const MAX_DISCOUNT_BPS = 2500;

 {
  const result: any = {};

  // Token amount (reuse existing regex)
  const amountMatch = text.match(
    /(\d+(?:,\d{3})*(?:\.\d+)?)\s*(?:tokens?|eliza)?/i,
  );
  if (amountMatch) {
    result.tokenAmount = amountMatch[1].replace(/,/g, "");
  }

  // Discount request
  const discountMatch = text.match(
    /(\d+(?:\.\d+)?)\s*(?:%|percent|bps|basis|discount)/i,
  );
  if (discountMatch) {
    const value = parseFloat(discountMatch[1]);
    if (
      discountMatch[0].includes("bps") ||
      discountMatch[0].includes("basis")
    ) {
      result.requestedDiscountBps = Math.round(value);
    } else {
      result.requestedDiscountBps = Math.round(value * 100);
    }
  }

  // Lockup period
  const lockupMatch = text.match(
    /(\d+)\s*(?:month|months|mo|week|weeks|wk|day|days|d)/i,
  );
  if (lockupMatch) {
    const value = parseInt(lockupMatch[1]);
    const unit = lockupMatch[0].toLowerCase();
    if (unit.includes("month") || unit.includes("mo")) {
      result.lockupMonths = value;
    } else if (unit.includes("week") || unit.includes("wk")) {
      result.lockupMonths = value / 4; // approx weeks->months
    } else if (unit.includes("day") || unit.includes("d")) {
      result.lockupMonths = value / 30; // approx days->months
    }
  }

  // Payment currency
  if (/(\b(eth|ethereum)\b)/i.test(text)) {
    result.paymentCurrency = "ETH";
  } else if (/(\b(usdc|usd|dollar)\b)/i.test(text)) {
    result.paymentCurrency = "USDC";
  }

  return result;
}

std::future<string | null> extractTokenContext(const std::string& text);()|[\]\\]/g, "\\$&")}\\b`,
      "i",
    );
    if (symbolRegex.test(text)) {
      return token.id;
    }

    // Also try matching with $ prefix (e.g., "$ELIZA")
    const dollarRegex = new RegExp(
      `\\$${token.symbol.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")}\\b`,
      "i",
    );
    if (dollarRegex.test(text)) {
      return token.id;
    }

    // Also match by name (case-insensitive)
    if (token.name && normalizedText.includes(token.name.toLowerCase())) {
      return token.id;
    }
  }

  // Fallback: if only one token is registered, use it
  if (allTokens.length === 1) {
    return allTokens[0].id;
  }

  return null;
}

std::future<OTCConsignment | null> findSuitableConsignment(const std::string& tokenId, const std::string& tokenAmount, double discountBps, double lockupDays););
  return consignmentService.findSuitableConsignment(
    consignments,
    tokenAmount,
    discountBps,
    lockupDays,
  );
}

// Worst possible deal defaults (lowest discount, longest lockup)
const DEFAULT_MIN_DISCOUNT_BPS = 100; // 1% - lowest discount
const DEFAULT_MAX_LOCKUP_MONTHS = 12; // 12 months - longest lockup
const DEFAULT_MAX_LOCKUP_DAYS = 365;

std::future<{
  lockupMonths: number;
  discountBps: number;
  paymentCurrency: "ETH" | "USDC";
  reasoning: string;
  consignmentId?: string;
}> negotiateTerms(IAgentRuntime _runtime, const std::any& request, const std::any& existingQuote, std::optional<OTCConsignment> consignment); else {
      const discountBps = consignment.fixedDiscountBps ?? 1000;
      const lockupDays = consignment.fixedLockupDays ?? 30;
      const lockupMonths = Math.round(lockupDays / 30);
      return {
        lockupMonths,
        discountBps,
        paymentCurrency: request.paymentCurrency || "USDC",
        reasoning: `This is a fixed-price deal: ${discountBps / 100}% discount with ${lockupDays} days lockup.`,
        consignmentId: consignment.id,
      };
    }
  }

  if (request.lockupMonths) {
    lockupMonths = Math.max(
      minLockupDays / 30,
      Math.min(maxLockupDays / 30, request.lockupMonths),
    );
  }

  let discountBps =
    request.requestedDiscountBps ??
    existingQuote?.discountBps ??
    DEFAULT_MIN_DISCOUNT_BPS;
  discountBps = Math.max(minDiscountBps, Math.min(maxDiscountBps, discountBps));

  if (discountBps >= 2000 && lockupMonths < 6) lockupMonths = 6;
  if (discountBps >= 2500 && lockupMonths < 9) lockupMonths = 9;

  const reasoning = `I can offer a ${(discountBps / 100).toFixed(2)}% discount with a ${lockupMonths}-month lockup.`;

  return {
    lockupMonths,
    discountBps,
    paymentCurrency:
      request.paymentCurrency || existingQuote?.paymentCurrency || "USDC",
    reasoning,
    consignmentId: consignment?.id,
  };
}

const quoteAction: Action = {
  name: "CREATE_OTC_QUOTE",
  similes: [
    "CREATE_OTC_QUOTE",
    "generate quote",
    "create quote",
    "new quote",
    "update quote",
    "modify quote",
  ],
  description: "Generate or update a quote with negotiated terms",

  validate: async () => true,

  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    state: State | undefined,
    _options?: { [key: string]: unknown },
    callback?: HandlerCallback,
  ): Promise<ActionResult> => {
    console.log("[CREATE_OTC_QUOTE] Action handler called");
    console.log(
      "[CREATE_OTC_QUOTE] Message object:",
      JSON.stringify(message, null, 2),
    );
    const entityId = message.entityId || message.roomId || "default";
    const text = message.content?.text || "";
    console.log(
      "[CREATE_OTC_QUOTE] EntityId:",
      entityId,
      "Text:",
      text.substring(0, 50),
    );

    // Check for quote cancellation
    if (
      text.toLowerCase().includes("cancel") ||
      text.toLowerCase().includes("delete")
    ) {
      const existingQuote = await getUserQuote(entityId);
      if (existingQuote) {
        await deleteUserQuote(entityId);
      }

      if (callback) {
        await callback({
          text: "Your quote has been cancelled.",
          action: "QUOTE_CANCELLED",
        });
      }
      return { success: true };
    }

    // Parse the request(s)
    const request = parseQuoteRequest(text);
    const negotiationRequest = parseNegotiationRequest(text);

    const tokenId = await extractTokenContext(text);
    const existingQuote = await getUserQuote(entityId);

    // Fetch token info for dynamic symbol/name
    let tokenSymbol = "TOKEN";
    let tokenName = "Token";
    let tokenChain: "evm" | "solana" | undefined = undefined;
    let tokenLogoUrl: string | undefined = undefined;
    if (tokenId) {
      const token = await TokenDB.getToken(tokenId);
      tokenSymbol = token.symbol;
      tokenName = token.name;
      tokenChain = token.chain === "solana" ? "solana" : "evm";
      tokenLogoUrl = token.logoUrl;
    }

    let consignment: OTCConsignment | null = null;
    if (tokenId && negotiationRequest.tokenAmount) {
      const lockupDays =
        (negotiationRequest.lockupMonths || DEFAULT_MAX_LOCKUP_MONTHS) * 30;
      consignment = await findSuitableConsignment(
        tokenId,
        negotiationRequest.tokenAmount,
        negotiationRequest.requestedDiscountBps || DEFAULT_MIN_DISCOUNT_BPS,
        lockupDays,
      );
    }

    const isNegotiation =
      /negotiate|discount|lockup|month/i.test(text) ||
      negotiationRequest.requestedDiscountBps !== undefined ||
      negotiationRequest.lockupMonths !== undefined;

    if (isNegotiation) {
      const negotiated = await negotiateTerms(
        runtime,
        negotiationRequest,
        existingQuote,
        consignment || undefined,
      );

      const ethPriceUsd =
        negotiated.paymentCurrency === "ETH" ? await getEthPriceUsd() : 0;

      // Generate terms-only quote
      const now = Date.now();
      const lockupDays = Math.round(negotiated.lockupMonths * 30);

      const quote = {
        tokenAmount: "0",
        discountBps: negotiated.discountBps,
        paymentCurrency: negotiated.paymentCurrency,
        priceUsdPerToken: 0, // Will be determined by Chainlink oracle on-chain
        totalUsd: 0,
        discountedUsd: 0,
        createdAt: now,
        quoteId: "", // Will be generated by service
        apr: 0,
        lockupMonths: negotiated.lockupMonths,
        paymentAmount: "0",
        // Token metadata
        tokenId: tokenId || undefined,
        tokenSymbol,
        tokenName,
        tokenLogoUrl,
        chain: tokenChain,
        consignmentId: negotiated.consignmentId,
      };

      console.log("[CREATE_OTC_QUOTE] Creating quote with negotiated terms");
      const storedQuote = await setUserQuote(entityId, quote);
      const quoteId = storedQuote.quoteId;
      console.log("[CREATE_OTC_QUOTE] Quote created with ID:", quoteId);

      const xmlResponse = `
<quote>
  <quoteId>${quoteId}</quoteId>
  <tokenSymbol>${tokenSymbol}</tokenSymbol>
  ${tokenChain ? `<tokenChain>${tokenChain}</tokenChain>` : ""}
  <lockupMonths>${negotiated.lockupMonths}</lockupMonths>
  <lockupDays>${lockupDays}</lockupDays>
  <pricePerToken>0</pricePerToken>
  <discountBps>${negotiated.discountBps}</discountBps>
  <discountPercent>${(negotiated.discountBps / 100).toFixed(2)}</discountPercent>
  <paymentCurrency>${negotiated.paymentCurrency}</paymentCurrency>
  ${negotiated.paymentCurrency === "ETH" ? `<ethPrice>${ethPriceUsd.toFixed(2)}</ethPrice>` : ""}
  <createdAt>${new Date(now).toISOString()}</createdAt>
  <status>negotiated</status>
  <message>Price will be determined by Chainlink oracle when you create the offer on-chain. Terms will be validated on-chain.</message>
</quote>`;

      const textResponse = `${negotiated.reasoning}

📊 **Quote Terms: Discount: ${(negotiated.discountBps / 100).toFixed(2)}% Lockup: ${negotiated.lockupMonths} months** (${lockupDays} days)`;

      if (callback) {
        await callback({
          text:
            textResponse +
            "\n\n<!-- XML_START -->\n" +
            xmlResponse +
            "\n<!-- XML_END -->",
          action: "QUOTE_NEGOTIATED",
          content: { xml: xmlResponse, quote, type: "otc_quote" } as Content,
        });
      }

      return { success: true };
    }

    // ------------- Simple discount-based quote -------------
    const discountBps =
      request.discountBps ??
      existingQuote?.discountBps ??
      DEFAULT_MIN_DISCOUNT_BPS; // Default worst deal (1%)
    const paymentCurrency =
      request.paymentCurrency || existingQuote?.paymentCurrency || "USDC";

    if (discountBps < 0 || discountBps > MAX_DISCOUNT_BPS) {
      if (callback) {
        await callback({
          text: "❌ Invalid discount. Please specify a discount between 0% and 25%.",
          action: "QUOTE_ERROR",
        });
      }
      return { success: false };
    }

    const ethPriceUsd = paymentCurrency === "ETH" ? await getEthPriceUsd() : 0;

    const now = Date.now();

    const quote = {
      tokenAmount: "0",
      discountBps,
      paymentCurrency,
      totalUsd: 0,
      discountedUsd: 0,
      createdAt: now,
      quoteId: "", // Will be generated by service
      apr: 0,
      lockupMonths: DEFAULT_MAX_LOCKUP_MONTHS, // Worst deal (12 months)
      paymentAmount: "0",
      // Token metadata
      tokenId: tokenId || undefined,
      tokenSymbol,
      tokenName,
      tokenLogoUrl,
      chain: tokenChain,
    };

    console.log("[CREATE_OTC_QUOTE] Creating quote with simple terms");
    const storedQuote = await setUserQuote(entityId, quote);
    const quoteId = storedQuote.quoteId;
    console.log("[CREATE_OTC_QUOTE] Quote created with ID:", quoteId);

    const xmlResponse = `
<quote>
  <quoteId>${quoteId}</quoteId>
  <tokenSymbol>${tokenSymbol}</tokenSymbol>
  <tokenName>${tokenName}</tokenName>
  ${tokenChain ? `<tokenChain>${tokenChain}</tokenChain>` : ""}
  <lockupMonths>${DEFAULT_MAX_LOCKUP_MONTHS}</lockupMonths>
  <lockupDays>${DEFAULT_MAX_LOCKUP_DAYS}</lockupDays>
  <pricePerToken>0</pricePerToken>
  <discountBps>${discountBps}</discountBps>
  <discountPercent>${(discountBps / 100).toFixed(2)}</discountPercent>
  <paymentCurrency>${paymentCurrency}</paymentCurrency>
  ${paymentCurrency === "ETH" ? `<ethPrice>${ethPriceUsd.toFixed(2)}</ethPrice>` : ""}
  <createdAt>${new Date(now).toISOString()}</createdAt>
  <status>created</status>
  <message>OTC quote terms generated. Price will be determined by Chainlink oracle when you create the offer on-chain.</message>
</quote>`;

    const textResponse = `I can offer a ${(discountBps / 100).toFixed(2)}% discount with a ${DEFAULT_MAX_LOCKUP_MONTHS}-month lockup.

📊 **Quote Terms: Discount: ${(discountBps / 100).toFixed(2)}% Lockup: ${DEFAULT_MAX_LOCKUP_MONTHS} months** (${DEFAULT_MAX_LOCKUP_DAYS} days)`;

    if (callback) {
      await callback({
        text:
          textResponse +
          "\n\n<!-- XML_START -->\n" +
          xmlResponse +
          "\n<!-- XML_END -->",
        action: "QUOTE_GENERATED",
        content: { xml: xmlResponse, quote, type: "otc_quote" } as Content,
      });
    }

    return { success: true };
  },

  examples: [],
};

} // namespace elizaos
