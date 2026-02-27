#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_UTILS_XML_PARSER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_UTILS_XML_PARSER_H
#include "core.hpp"

class OTCQuote;
class QuoteAccepted;

class OTCQuote : public object, public std::enable_shared_from_this<OTCQuote> {
public:
    using std::enable_shared_from_this<OTCQuote>::shared_from_this;
    string quoteId;

    string beneficiary;

    string tokenAmount;

    string tokenAmountFormatted;

    string tokenSymbol;

    any tokenChain;

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

    string paymentCurrency;

    string paymentAmount;

    string paymentSymbol;

    double ethPrice;

    string createdAt;

    string status;

    string message;

    string consignmentId;

    boolean isFixedPrice;
};

class QuoteAccepted : public object, public std::enable_shared_from_this<QuoteAccepted> {
public:
    using std::enable_shared_from_this<QuoteAccepted>::shared_from_this;
    string quoteId;

    string offerId;

    string transactionHash;

    string tokenAmount;

    string tokenAmountFormatted;

    string tokenSymbol;

    string tokenName;

    string paidAmount;

    string paymentCurrency;

    double discountBps;

    double discountPercent;

    string totalSaved;

    string finalPrice;

    string status;

    string timestamp;

    string message;
};

any extractXMLFromMessage(string messageText);

any parseOTCQuoteXML(string xmlString);

any parseQuoteAcceptedXML(string xmlString);

boolean messageContainsQuote(string messageText);

object parseMessageXML(string messageText);

#endif
