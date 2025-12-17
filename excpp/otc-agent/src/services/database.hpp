#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Database service layer using Eliza runtime services

class QuoteDB {
public:
    static std::future<Quote> createQuote(const std::any& data);
    static std::future<std::vector<Quote>> getActiveQuotes();
    static std::future<Quote> getQuoteByBeneficiary(const std::string& beneficiary);
    static std::future<Quote> getQuoteByQuoteId(const std::string& quoteId);
    static std::future<Quote> updateQuoteStatus(const std::string& quoteId, QuoteStatus status, const std::any& data);
    static std::future<Quote> updateQuoteExecution(const std::string& quoteId, std::optional<std::any> data);
    static std::future<Quote> setQuoteBeneficiary(const std::string& quoteId, const std::string& beneficiary);
    static std::future<std::vector<Quote>> getUserQuoteHistory(const std::string& entityId, double limit);
    static std::future<bool> verifyQuoteSignature(Quote quote);

private:
    std::string entityId_;
    std::string beneficiary_;
    std::string tokenAmount_;
    double discountBps_;
    double apr_;
    double lockupMonths_;
    PaymentCurrency paymentCurrency_;
    double totalUsd_;
    double discountUsd_;
    double discountedUsd_;
    std::string paymentAmount_;
    std::string offerId_;
    std::string transactionHash_;
    double blockNumber_;
    std::string rejectionReason_;
    std::string approvalNote_;
    std::string tokenAmount_;
    double totalUsd_;
    double discountUsd_;
    double discountedUsd_;
    PaymentCurrency paymentCurrency_;
    std::string paymentAmount_;
    std::string offerId_;
    std::string transactionHash_;
    double blockNumber_;
};

class DealCompletionService {
public:
    static void generateShareData(const std::string& quoteId);
};

/**
 * Normalizes a tokenId to ensure consistent lookups.
 * EVM addresses are case-insensitive, so they are lowercased.
 * Solana addresses (Base58) are case-sensitive, so they are preserved.
 * Format: token-{chain}-{address}
 */
std::string normalizeTokenId(const std::string& tokenId);

class TokenDB {
public:
    static std::future<Token> getToken(const std::string& tokenId);
    static std::future<std::vector<Token>> getAllTokens(std::optional<std::any> filters);
    static std::future<Token> updateToken(const std::string& tokenId, const std::optional<Token>& updates);
    static std::variant<Promise<Token, null>> getTokenByOnChainId(const std::string& onChainTokenId);
    static std::variant<Promise<Token, null>> getTokenBySymbol(const std::string& symbol);
};

class MarketDataDB {
public:
    static std::future<void> setMarketData(TokenMarketData data);
    static std::variant<Promise<TokenMarketData, null>> getMarketData(const std::string& tokenId);
};

class ConsignmentDB {
public:
    static std::future<OTCConsignment> getConsignment(const std::string& consignmentId);
    static std::future<OTCConsignment> updateConsignment(const std::string& consignmentId, const std::optional<OTCConsignment>& updates);
    static std::future<std::vector<OTCConsignment>> getConsignmentsByToken(const std::string& tokenId);
    static std::future<std::vector<OTCConsignment>> getConsignmentsByConsigner(const std::string& consignerAddress, auto includeWithdrawn);
    static std::future<std::vector<OTCConsignment>> getAllConsignments(std::optional<std::any> filters);
};

class ConsignmentDealDB {
public:
    static std::future<std::vector<ConsignmentDeal>> getDealsByConsignment(const std::string& consignmentId);
};


} // namespace elizaos
