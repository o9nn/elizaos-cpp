#include "xml-parser.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractXMLFromMessage(const std::string& messageText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Try to find XML between comment markers first
    const auto commentMatch = messageText.match(;
    /<!-- XML_START -->([\s\S]*?)<!-- XML_END -->/,
    );
    if (commentMatch && commentMatch[1]) {
        return commentMatch[1].trim();
    }

    // Try to find quote XML (supports lower and PascalCase)
    const auto quoteMatch = messageText.match(;
    /<(quote|Quote)>([\s\S]*?)<\/(quote|Quote)>/,
    );
    if (quoteMatch && quoteMatch[0]) {
        return quoteMatch[0];
    }

    // Try to find quoteAccepted XML (supports lower and PascalCase)
    const auto acceptedMatch = messageText.match(;
    /<(quoteAccepted|QuoteAccepted)>([\s\S]*?)<\/(quoteAccepted|QuoteAccepted)>/,
    );
    if (acceptedMatch && acceptedMatch[0]) {
        return acceptedMatch[0];
    }

    return nullptr;

}

std::optional<OTCQuote> parseOTCQuoteXML(const std::string& xmlString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parser = new DOMParser();
    const auto xmlDoc = parser.parseFromstd::to_string(xmlString, "text/xml");

    // Check for parsing errors
    const auto parseError = xmlDoc.querySelector("parsererror");
    if (parseError) {
        std::cerr << "XML parsing error:" << parseError.textContent << std::endl;
        return nullptr;
    }

    const auto getElementText = (tagName: string): string => {;
        const auto elem = xmlDoc.getElementsByTagName(tagName)[0];
        return elem ? elem.textContent || "" : "";
        };

        const auto getElementNumber = (tagName: string): number => {;
            const auto text = getElementText(tagName);
            return text ? parseFloat(text) : 0;
            };

            // Support both lowercase and PascalCase root tags
            const auto rootTag =;
            xmlDoc.querySelector("Quote") || xmlDoc.querySelector("quote");
            if (!rootTag) {
                std::cerr << "No quote root element found" << std::endl;
                return nullptr;
            }

            const auto tokenChain = getElementText("tokenChain") || getElementText("chain");

            return {
                quoteId: getElementText("quoteId"),
                tokenAmount: getElementText("tokenAmount"),
                tokenAmountFormatted: getElementText("tokenAmountFormatted"),
                tokenSymbol: getElementText("tokenSymbol"),
                tokenChain: tokenChain
                ? (tokenChain as "ethereum" | "base" | "bsc" | "solana");
                : std::nullopt,
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
                ethPrice: getElementNumber("ethPrice") || std::nullopt,
                createdAt: getElementText("createdAt"),
                status: getElementText("status") || std::nullopt,
                message: getElementText("message"),
                };

}

std::optional<QuoteAccepted> parseQuoteAcceptedXML(const std::string& xmlString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parser = new DOMParser();
    const auto xmlDoc = parser.parseFromstd::to_string(xmlString, "text/xml");

    // Check for parsing errors
    const auto parseError = xmlDoc.querySelector("parsererror");
    if (parseError) {
        std::cerr << "XML parsing error:" << parseError.textContent << std::endl;
        return nullptr;
    }

    const auto getElementText = (tagName: string): string => {;
        const auto elem = xmlDoc.getElementsByTagName(tagName)[0];
        return elem ? elem.textContent || "" : "";
        };

        const auto getElementNumber = (tagName: string): number => {;
            const auto text = getElementText(tagName);
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

bool messageContainsQuote(const std::string& messageText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return !!(;
    (std::find(messageText.begin(), messageText.end(), "<quote>") != messageText.end()) ||;
    (std::find(messageText.begin(), messageText.end(), "<quote>") != messageText.end()) ||;
    (std::find(messageText.begin(), messageText.end(), "<quoteAccepted>") != messageText.end()) ||;
    (std::find(messageText.begin(), messageText.end(), "<!-- XML_START -->") != messageText.end());
    );

}

void parseMessageXML(const std::string& messageText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    type: "otc_quote" | "quote_accepted" | nullptr;
    data: OTCQuote | QuoteAccepted | nullptr;

}

} // namespace elizaos
