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
    std::string quoteId;
    std::optional<std::string> beneficiary;
    std::string tokenAmount;
    std::optional<std::string> tokenAmountFormatted;
    std::string tokenSymbol;
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
std::optional<std::string> extractXMLFromMessage(const std::string& messageText);

/**
 * Parse quote from XML
 */
std::optional<OTCQuote> parseOTCQuoteXML(const std::string& xmlString);

/**
 * Parse quote accepted XML
 */
std::optional<QuoteAccepted> parseQuoteAcceptedXML(const std::string& xmlString);

/**
 * Check if message contains a quote
 */
bool messageContainsQuote(const std::string& messageText);

/**
 * Parse any XML type from message
 */
void parseMessageXML(const std::string& messageText); {

  // Try parsing as quote

  // Try parsing as quote accepted


} // namespace elizaos
