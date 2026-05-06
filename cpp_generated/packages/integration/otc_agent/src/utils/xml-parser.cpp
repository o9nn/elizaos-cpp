#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/utils/xml-parser.h"

any extractXMLFromMessage(string messageText)
{
    auto commentMatch = messageText->match((new RegExp(std::string("<!-- XML_START -->([\s\S]*?)<!-- XML_END --"))));
    if (AND((commentMatch), ((*const_(commentMatch))[1]))) {
        return (*const_(commentMatch))[1]->trim();
    }
    auto quoteMatch = messageText->match((new RegExp(std::string("<(quote|Quote)>([\s\S]*?)<\/(quote|Quote)"))));
    if (AND((quoteMatch), ((*const_(quoteMatch))[0]))) {
        return (*const_(quoteMatch))[0];
    }
    auto acceptedMatch = messageText->match((new RegExp(std::string("<(quoteAccepted|QuoteAccepted)>([\s\S]*?)<\/(quoteAccepted|QuoteAccepted)"))));
    if (AND((acceptedMatch), ((*const_(acceptedMatch))[0]))) {
        return (*const_(acceptedMatch))[0];
    }
    return nullptr;
};


any parseOTCQuoteXML(string xmlString)
{
    auto parser = std::make_shared<DOMParser>();
    shared xmlDoc = parser->parseFromString(xmlString, std::string("text/xml"));
    auto parseError = xmlDoc->querySelector(std::string("parsererror"));
    if (parseError) {
        console->error(std::string("XML parsing error:"), parseError->textContent);
        return nullptr;
    }
    shared getElementText = [=](auto tagName) mutable
    {
        auto elem = const_(xmlDoc->getElementsByTagName(tagName))[0];
        return (elem) ? any(OR((elem->textContent), (string_empty))) : any(string_empty);
    };
    auto getElementNumber = [=](auto tagName) mutable
    {
        auto text = getElementText(tagName);
        return (text) ? any(parseFloat(text)) : any(0);
    };
    auto rootTag = OR((xmlDoc->querySelector(std::string("Quote"))), (xmlDoc->querySelector(std::string("quote"))));
    if (!rootTag) {
        console->error(std::string("No quote root element found"));
        return nullptr;
    }
    auto tokenChain = OR((getElementText(std::string("tokenChain"))), (getElementText(std::string("chain"))));
    return object{
        object::pair{std::string("quoteId"), getElementText(std::string("quoteId"))}, 
        object::pair{std::string("tokenAmount"), getElementText(std::string("tokenAmount"))}, 
        object::pair{std::string("tokenAmountFormatted"), getElementText(std::string("tokenAmountFormatted"))}, 
        object::pair{std::string("tokenSymbol"), getElementText(std::string("tokenSymbol"))}, 
        object::pair{std::string("tokenChain"), (tokenChain) ? any((as<any>(tokenChain))) : any(undefined)}, 
        object::pair{std::string("apr"), getElementNumber(std::string("apr"))}, 
        object::pair{std::string("lockupMonths"), getElementNumber(std::string("lockupMonths"))}, 
        object::pair{std::string("lockupDays"), getElementNumber(std::string("lockupDays"))}, 
        object::pair{std::string("pricePerToken"), OR((getElementNumber(std::string("pricePerToken"))), (getElementNumber(std::string("priceUsdPerToken"))))}, 
        object::pair{std::string("totalValueUsd"), getElementNumber(std::string("totalValueUsd"))}, 
        object::pair{std::string("discountBps"), getElementNumber(std::string("discountBps"))}, 
        object::pair{std::string("discountPercent"), getElementNumber(std::string("discountPercent"))}, 
        object::pair{std::string("discountUsd"), getElementNumber(std::string("discountUsd"))}, 
        object::pair{std::string("finalPriceUsd"), OR((getElementNumber(std::string("finalPriceUsd"))), (getElementNumber(std::string("discountedUsd"))))}, 
        object::pair{std::string("paymentCurrency"), getElementText(std::string("paymentCurrency"))}, 
        object::pair{std::string("paymentAmount"), getElementText(std::string("paymentAmount"))}, 
        object::pair{std::string("paymentSymbol"), getElementText(std::string("paymentSymbol"))}, 
        object::pair{std::string("ethPrice"), OR((getElementNumber(std::string("ethPrice"))), (undefined))}, 
        object::pair{std::string("createdAt"), getElementText(std::string("createdAt"))}, 
        object::pair{std::string("status"), OR((getElementText(std::string("status"))), (undefined))}, 
        object::pair{std::string("message"), getElementText(std::string("message"))}
    };
};


any parseQuoteAcceptedXML(string xmlString)
{
    auto parser = std::make_shared<DOMParser>();
    shared xmlDoc = parser->parseFromString(xmlString, std::string("text/xml"));
    auto parseError = xmlDoc->querySelector(std::string("parsererror"));
    if (parseError) {
        console->error(std::string("XML parsing error:"), parseError->textContent);
        return nullptr;
    }
    shared getElementText = [=](auto tagName) mutable
    {
        auto elem = const_(xmlDoc->getElementsByTagName(tagName))[0];
        return (elem) ? any(OR((elem->textContent), (string_empty))) : any(string_empty);
    };
    auto getElementNumber = [=](auto tagName) mutable
    {
        auto text = getElementText(tagName);
        return (text) ? any(parseFloat(text)) : any(0);
    };
    return object{
        object::pair{std::string("quoteId"), getElementText(std::string("quoteId"))}, 
        object::pair{std::string("offerId"), getElementText(std::string("offerId"))}, 
        object::pair{std::string("transactionHash"), getElementText(std::string("transactionHash"))}, 
        object::pair{std::string("tokenAmount"), getElementText(std::string("tokenAmount"))}, 
        object::pair{std::string("tokenAmountFormatted"), getElementText(std::string("tokenAmountFormatted"))}, 
        object::pair{std::string("tokenSymbol"), getElementText(std::string("tokenSymbol"))}, 
        object::pair{std::string("tokenName"), getElementText(std::string("tokenName"))}, 
        object::pair{std::string("paidAmount"), getElementText(std::string("paidAmount"))}, 
        object::pair{std::string("paymentCurrency"), getElementText(std::string("paymentCurrency"))}, 
        object::pair{std::string("discountBps"), getElementNumber(std::string("discountBps"))}, 
        object::pair{std::string("discountPercent"), getElementNumber(std::string("discountPercent"))}, 
        object::pair{std::string("totalSaved"), getElementText(std::string("totalSaved"))}, 
        object::pair{std::string("finalPrice"), getElementText(std::string("finalPrice"))}, 
        object::pair{std::string("status"), getElementText(std::string("status"))}, 
        object::pair{std::string("timestamp"), getElementText(std::string("timestamp"))}, 
        object::pair{std::string("message"), getElementText(std::string("message"))}
    };
};


boolean messageContainsQuote(string messageText)
{
    return !!(OR((OR((OR((messageText->includes(std::string("<quote>"))), (messageText->includes(std::string("<quote>"))))), (messageText->includes(std::string("<quoteAccepted>"))))), (messageText->includes(std::string("<!-- XML_START -->")))));
};


object parseMessageXML(string messageText)
{
    auto xmlString = extractXMLFromMessage(messageText);
    if (!xmlString) {
        return object{
            object::pair{std::string("type"), nullptr}, 
            object::pair{std::string("data"), nullptr}
        };
    }
    if (xmlString->match((new RegExp(std::string("<(quote|Quote)"))))) {
        auto quote = parseOTCQuoteXML(xmlString);
        if (quote) {
            return object{
                object::pair{std::string("type"), std::string("otc_quote")}, 
                object::pair{std::string("data"), quote}
            };
        }
    }
    if (xmlString->match((new RegExp(std::string("<(quoteAccepted|QuoteAccepted)"))))) {
        auto accepted = parseQuoteAcceptedXML(xmlString);
        if (accepted) {
            return object{
                object::pair{std::string("type"), std::string("quote_accepted")}, 
                object::pair{std::string("data"), accepted}
            };
        }
    }
    return object{
        object::pair{std::string("type"), nullptr}, 
        object::pair{std::string("data"), nullptr}
    };
};


