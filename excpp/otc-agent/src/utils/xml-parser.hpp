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

/**
 * Utility to parse XML from agent messages
 */

struct OTCQuote {
    std::string quoteId;
    std::optional<std::string> beneficiary;
    std::string tokenAmount;
    std::optional<std::string> tokenAmountFormatted;
    std::string tokenSymbol;
    std::optional<"ethereum" | "base" | "bsc" | "solana"> tokenChain;
    std::optional<double> apr;
    double lockupMonths;
    double lockupDays;
    std::optional<double> pricePerToken;
    std::optional<double> priceUsd;
    std::optional<double> totalValueUsd;
    std::optional<double> totalUsd;
    double discountBps;
    double discountPercent;
    std::optional<double> discountUsd;
    std::optional<double> finalPriceUsd;
    std::string paymentCurrency;
    std::optional<std::string> paymentAmount;
    std::optional<std::string> paymentSymbol;
    std::optional<double> ethPrice;
    std::optional<std::string> createdAt;
    std::optional<std::string> status;
    std::optional<std::string> message;
    std::optional<std::string> consignmentId;
    std::optional<bool> isFixedPrice;
};


struct QuoteAccepted {
    std::string quoteId;
    std::string offerId;
    std::string transactionHash;
    std::string tokenAmount;
    std::string tokenAmountFormatted;
    std::string tokenSymbol;
    std::string tokenName;
    std::string paidAmount;
    std::string paymentCurrency;
    double discountBps;
    double discountPercent;
    std::string totalSaved;
    std::string finalPrice;
    std::string status;
    std::string timestamp;
    std::string message;
};


/**
 * Extract XML from message text
 */


  // Try to find quote XML (supports lower and PascalCase)
  const quoteMatch = messageText.match(
    /<(quote|Quote)>([\s\S]*?)<\/(quote|Quote)>/,
  );
  if (quoteMatch && quoteMatch[0]) {
    return quoteMatch[0];
  }

  // Try to find quoteAccepted XML (supports lower and PascalCase)
  const acceptedMatch = messageText.match(
    /<(quoteAccepted|QuoteAccepted)>([\s\S]*?)<\/(quoteAccepted|QuoteAccepted)>/,
  );
  if (acceptedMatch && acceptedMatch[0]) {
    return acceptedMatch[0];
  }

  return null;
}

/**
 * Parse quote from XML
 */


  const getElementText = (tagName: string): string => {
    const elem = xmlDoc.getElementsByTagName(tagName)[0];
    return elem ? elem.textContent || "" : "";
  };

  const getElementNumber = (tagName: string): number => {
    const text = getElementText(tagName);
    return text ? parseFloat(text) : 0;
  };

  // Support both lowercase and PascalCase root tags
  const rootTag =
    xmlDoc.querySelector("Quote") || xmlDoc.querySelector("quote");
  if (!rootTag) {
    console.error("No quote root element found");
    return null;
  }

  const tokenChain = getElementText("tokenChain") || getElementText("chain");

  return {
    quoteId: getElementText("quoteId"),
    tokenAmount: getElementText("tokenAmount"),
    tokenAmountFormatted: getElementText("tokenAmountFormatted"),
    tokenSymbol: getElementText("tokenSymbol"),
    tokenChain: tokenChain
      ? (tokenChain as "ethereum" | "base" | "bsc" | "solana")
      : undefined,
    apr: getElementNumber("apr"),
    lockupMonths: getElementNumber("lockupMonths"),
    lockupDays: getElementNumber("lockupDays"),
    pricePerToken:
      getElementNumber("pricePerToken") || getElementNumber("priceUsdPerToken"),
    totalValueUsd: getElementNumber("totalValueUsd"),
    discountBps: getElementNumber("discountBps"),
    discountPercent: getElementNumber("discountPercent"),
    discountUsd: getElementNumber("discountUsd"),
    finalPriceUsd:
      getElementNumber("finalPriceUsd") || getElementNumber("discountedUsd"),
    paymentCurrency: getElementText("paymentCurrency"),
    paymentAmount: getElementText("paymentAmount"),
    paymentSymbol: getElementText("paymentSymbol"),
    ethPrice: getElementNumber("ethPrice") || undefined,
    createdAt: getElementText("createdAt"),
    status: getElementText("status") || undefined,
    message: getElementText("message"),
  };
}

/**
 * Parse quote accepted XML
 */


  const getElementText = (tagName: string): string => {
    const elem = xmlDoc.getElementsByTagName(tagName)[0];
    return elem ? elem.textContent || "" : "";
  };

  const getElementNumber = (tagName: string): number => {
    const text = getElementText(tagName);
    return text ? parseFloat(text) : 0;
  };

  return {
    quoteId: getElementText("quoteId"),
    offerId: getElementText("offerId"),
    transactionHash: getElementText("transactionHash"),
    tokenAmount: getElementText("tokenAmount"),
    tokenAmountFormatted: getElementText("tokenAmountFormatted"),
    tokenSymbol: getElementText("tokenSymbol"),
    tokenName: getElementText("tokenName"),
    paidAmount: getElementText("paidAmount"),
    paymentCurrency: getElementText("paymentCurrency"),
    discountBps: getElementNumber("discountBps"),
    discountPercent: getElementNumber("discountPercent"),
    totalSaved: getElementText("totalSaved"),
    finalPrice: getElementText("finalPrice"),
    status: getElementText("status"),
    timestamp: getElementText("timestamp"),
    message: getElementText("message"),
  };
}

/**
 * Check if message contains a quote
 */


/**
 * Parse any XML type from message
 */
 {
  const xmlString = extractXMLFromMessage(messageText);

  if (!xmlString) {
    return { type: null, data: null };
  }

  // Try parsing as quote
  if (xmlString.match(/<(quote|Quote)>/)) {
    const quote = parseOTCQuoteXML(xmlString);
    if (quote) {
      return { type: "otc_quote", data: quote };
    }
  }

  // Try parsing as quote accepted
  if (xmlString.match(/<(quoteAccepted|QuoteAccepted)>/)) {
    const accepted = parseQuoteAcceptedXML(xmlString);
    if (accepted) {
      return { type: "quote_accepted", data: accepted };
    }
  }

  return { type: null, data: null };
}

} // namespace elizaos
