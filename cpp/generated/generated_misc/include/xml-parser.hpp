#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_XML-PARSER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_XML-PARSER_H
#include "core.h"

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
