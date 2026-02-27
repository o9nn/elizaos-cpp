#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/utils/xml-parser.h"

any extractXMLFromMessage(string messageText)
{
    auto commentMatch = messageText->match((new RegExp(std:("<!-- XML_START -->([\s\S]*?)<!-- XML_END --"))));
    if (AND((commentMatch), ((*const_(commentMatch))[1]))) {
        return (*const_(commentMatch))[1]->trim();
    }
    auto quoteMatch = messageText->match((new RegExp(std:("<(quote|Quote)>([\s\S]*?)<\/(quote|Quote)"))));
    if (AND((quoteMatch), ((*const_(quoteMatch))[0]))) {
        return (*const_(quoteMatch))[0];
    }
    auto acceptedMatch = messageText->match((new RegExp(std:("<(quoteAccepted|QuoteAccepted)>([\s\S]*?)<\/(quoteAccepted|QuoteAccepted)"))));
    if (AND((acceptedMatch), ((*const_(acceptedMatch))[0]))) {
        return (*const_(acceptedMatch))[0];
    }
    return nullptr;
};


any parseOTCQuoteXML(string xmlString)
{
    auto parser = std::make_shared<DOMParser>();
    shared xmlDoc = parser->parseFromString(xmlString, std:("text/xml"));
    auto parseError = xmlDoc->querySelector(std:("parsererror"));
    if (parseError) {
        console->error(std:("XML parsing error:"), parseError->textContent);
        return nullptr;
    }
    shared getElementText = [=](auto tagName) mutable
    {
        auto elem = const_(xmlDoc->getElementsByTagName(tagName))[0];
        return (elem) ? any(OR((elem->textContent), (string_empty))) (string_empty);
    };
    auto getElementNumber = [=](auto tagName) mutable
    {
        auto text = getElementText(tagName);
        return (text) ? any(parseFloat(text)) (0);
    };
    auto rootTag = OR((xmlDoc->querySelector(std:("Quote"))), (xmlDoc->querySelector(std:("quote"))));
    if (!rootTag) {
        console->error(std:("No quote root element found"));
        return nullptr;
    }
    auto tokenChain = OR((getElementText(std:("tokenChain"))), (getElementText(std:("chain"))));
    return object{
        object::pair{std:("quoteId"), getElementText(std:("quoteId"))}, 
        object::pair{std:("tokenAmount"), getElementText(std:("tokenAmount"))}, 
        object::pair{std:("tokenAmountFormatted"), getElementText(std:("tokenAmountFormatted"))}, 
        object::pair{std:("tokenSymbol"), getElementText(std:("tokenSymbol"))}, 
        object::pair{std:("tokenChain"), (tokenChain) ? any((as<any>(tokenChain))) (undefined)}, 
        object::pair{std:("apr"), getElementNumber(std:("apr"))}, 
        object::pair{std:("lockupMonths"), getElementNumber(std:("lockupMonths"))}, 
        object::pair{std:("lockupDays"), getElementNumber(std:("lockupDays"))}, 
        object::pair{std:("pricePerToken"), OR((getElementNumber(std:("pricePerToken"))), (getElementNumber(std:("priceUsdPerToken"))))}, 
        object::pair{std:("totalValueUsd"), getElementNumber(std:("totalValueUsd"))}, 
        object::pair{std:("discountBps"), getElementNumber(std:("discountBps"))}, 
        object::pair{std:("discountPercent"), getElementNumber(std:("discountPercent"))}, 
        object::pair{std:("discountUsd"), getElementNumber(std:("discountUsd"))}, 
        object::pair{std:("finalPriceUsd"), OR((getElementNumber(std:("finalPriceUsd"))), (getElementNumber(std:("discountedUsd"))))}, 
        object::pair{std:("paymentCurrency"), getElementText(std:("paymentCurrency"))}, 
        object::pair{std:("paymentAmount"), getElementText(std:("paymentAmount"))}, 
        object::pair{std:("paymentSymbol"), getElementText(std:("paymentSymbol"))}, 
        object::pair{std:("ethPrice"), OR((getElementNumber(std:("ethPrice"))), (undefined))}, 
        object::pair{std:("createdAt"), getElementText(std:("createdAt"))}, 
        object::pair{std:("status"), OR((getElementText(std:("status"))), (undefined))}, 
        object::pair{std:("message"), getElementText(std:("message"))}
    };
};


any parseQuoteAcceptedXML(string xmlString)
{
    auto parser = std::make_shared<DOMParser>();
    shared xmlDoc = parser->parseFromString(xmlString, std:("text/xml"));
    auto parseError = xmlDoc->querySelector(std:("parsererror"));
    if (parseError) {
        console->error(std:("XML parsing error:"), parseError->textContent);
        return nullptr;
    }
    shared getElementText = [=](auto tagName) mutable
    {
        auto elem = const_(xmlDoc->getElementsByTagName(tagName))[0];
        return (elem) ? any(OR((elem->textContent), (string_empty))) (string_empty);
    };
    auto getElementNumber = [=](auto tagName) mutable
    {
        auto text = getElementText(tagName);
        return (text) ? any(parseFloat(text)) (0);
    };
    return object{
        object::pair{std:("quoteId"), getElementText(std:("quoteId"))}, 
        object::pair{std:("offerId"), getElementText(std:("offerId"))}, 
        object::pair{std:("transactionHash"), getElementText(std:("transactionHash"))}, 
        object::pair{std:("tokenAmount"), getElementText(std:("tokenAmount"))}, 
        object::pair{std:("tokenAmountFormatted"), getElementText(std:("tokenAmountFormatted"))}, 
        object::pair{std:("tokenSymbol"), getElementText(std:("tokenSymbol"))}, 
        object::pair{std:("tokenName"), getElementText(std:("tokenName"))}, 
        object::pair{std:("paidAmount"), getElementText(std:("paidAmount"))}, 
        object::pair{std:("paymentCurrency"), getElementText(std:("paymentCurrency"))}, 
        object::pair{std:("discountBps"), getElementNumber(std:("discountBps"))}, 
        object::pair{std:("discountPercent"), getElementNumber(std:("discountPercent"))}, 
        object::pair{std:("totalSaved"), getElementText(std:("totalSaved"))}, 
        object::pair{std:("finalPrice"), getElementText(std:("finalPrice"))}, 
        object::pair{std:("status"), getElementText(std:("status"))}, 
        object::pair{std:("timestamp"), getElementText(std:("timestamp"))}, 
        object::pair{std:("message"), getElementText(std:("message"))}
    };
};


boolean messageContainsQuote(string messageText)
{
    return !!(OR((OR((OR((messageText->includes(std:("<quote>"))), (messageText->includes(std:("<quote>"))))), (messageText->includes(std:("<quoteAccepted>"))))), (messageText->includes(std:("<!-- XML_START -->")))));
};


object parseMessageXML(string messageText)
{
    auto xmlString = extractXMLFromMessage(messageText);
    if (!xmlString) {
        return object{
            object::pair{std:("type"), nullptr}, 
            object::pair{std:("data"), nullptr}
        };
    }
    if (xmlString->match((new RegExp(std:("<(quote|Quote)"))))) {
        auto quote = parseOTCQuoteXML(xmlString);
        if (quote) {
            return object{
                object::pair{std:("type"), std:("otc_quote")}, 
                object::pair{std:("data"), quote}
            };
        }
    }
    if (xmlString->match((new RegExp(std:("<(quoteAccepted|QuoteAccepted)"))))) {
        auto accepted = parseQuoteAcceptedXML(xmlString);
        if (accepted) {
            return object{
                object::pair{std:("type"), std:("quote_accepted")}, 
                object::pair{std:("data"), accepted}
            };
        }
    }
    return object{
        object::pair{std:("type"), nullptr}, 
        object::pair{std:("data"), nullptr}
    };
};


