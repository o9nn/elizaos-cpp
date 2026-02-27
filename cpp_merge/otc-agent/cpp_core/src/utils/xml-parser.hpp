#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Utility to parse XML from agent messages
 */

struct OTCQuote {
    std: quoteId;
    std::optional<std:> beneficiary;
    std: tokenAmount;
    std::optional<std:> tokenAmountFormatted;
    std: tokenSymbol;
    std::optional<std::variant<"ethereum", "base", "bsc", "solana">> tokenChain;
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
    std: paymentCurrency;
    std::optional<std:> paymentAmount;
    std::optional<std:> paymentSymbol;
    std::optional<double> ethPrice;
    std::optional<std:> createdAt;
    std::optional<std:> status;
    std::optional<std:> message;
    std::optional<std:> consignmentId;
    std::optional<bool> isFixedPrice;
};

struct QuoteAccepted {
    std: quoteId;
    std: offerId;
    std: transactionHash;
    std: tokenAmount;
    std: tokenAmountFormatted;
    std: tokenSymbol;
    std: tokenName;
    std: paidAmount;
    std: paymentCurrency;
    double discountBps;
    double discountPercent;
    std: totalSaved;
    std: finalPrice;
    std: status;
    std: timestamp;
    std: message;
};

/**
 * Extract XML from message text
 */
std::optional<std:> extractXMLFromMessage(const std:& messageText);

/**
 * Parse quote from XML
 */
std::optional<OTCQuote> parseOTCQuoteXML(const std:& xmlString);

/**
 * Parse quote accepted XML
 */
std::optional<QuoteAccepted> parseQuoteAcceptedXML(const std:& xmlString);

/**
 * Check if message contains a quote
 */
bool messageContainsQuote(const std:& messageText);

/**
 * Parse std: XML type from message
 */
void parseMessageXML(const std:& messageText); {

  // Try parsing as quote

  // Try parsing as quote accepted


} // namespace elizaos
