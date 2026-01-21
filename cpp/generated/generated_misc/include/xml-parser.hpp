#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_XML-PARSER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_XML-PARSER_H
#include "core.h"

class OTCQuote;
class QuoteAccepted;

class OTCQuote : public object, public std::enable_shared_from_this<OTCQuote> {
public:
    using std::enable_shared_from_this<OTCQuote>::shared_from_this;
    std::string quoteId;

    std::string beneficiary;

    std::string tokenAmount;

    std::string tokenAmountFormatted;

    std::string tokenSymbol;

    std::any tokenChain;

    double apr;

    double lockupMonths;

    double lockupDays;

    double pricePerToken;

    double priceUsd;

    double totalValueUsd;

    double totalUsd;

    double discountBps;

    double discountPercent;

    double discountUsd;

    double finalPriceUsd;

    std::string paymentCurrency;

    std::string paymentAmount;

    std::string paymentSymbol;

    double ethPrice;

    std::string createdAt;

    std::string status;

    std::string message;

    std::string consignmentId;

    boolean isFixedPrice;
};

class QuoteAccepted : public object, public std::enable_shared_from_this<QuoteAccepted> {
public:
    using std::enable_shared_from_this<QuoteAccepted>::shared_from_this;
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

std::any extractXMLFromMessage(std::string messageText);

std::any parseOTCQuoteXML(std::string xmlString);

std::any parseQuoteAcceptedXML(std::string xmlString);

boolean messageContainsQuote(std::string messageText);

object parseMessageXML(std::string messageText);

#endif
