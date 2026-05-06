/**
 * Utility to parse XML from agent messages
 */

export interface OTCQuote {
  quoteId: string;
  beneficiary?: string;
  tokenAmount: string;
  tokenAmountFormatted?: string;
  tokenSymbol: string;
  tokenChain?: "ethereum" | "base" | "bsc" | "solana";
  apr?: number;
  lockupMonths: number;
  lockupDays: number;
  pricePerToken?: number;
  priceUsd?: number;
  totalValueUsd?: number;
  totalUsd?: number;
  discountBps: number;
  discountPercent: number;
  discountUsd?: number;
  finalPriceUsd?: number;
  paymentCurrency: string;
  paymentAmount?: string;
  paymentSymbol?: string;
  ethPrice?: number;
  createdAt?: string;
  status?: string;
  message?: string;
  consignmentId?: string;
  isFixedPrice?: boolean;
}

export interface QuoteAccepted {
  quoteId: string;
  offerId: string;
  transactionHash: string;
  tokenAmount: string;
  tokenAmountFormatted: string;
  tokenSymbol: string;
  tokenName: string;
  paidAmount: string;
  paymentCurrency: string;
  discountBps: number;
  discountPercent: number;
  totalSaved: string;
  finalPrice: string;
  status: string;
  timestamp: string;
  message: string;
}

/**
 * Extract XML from message text
 */
export function extractXMLFromMessage(messageText: string): string | null {
  // Try to find XML between comment markers first
  const commentMatch = messageText.match(
    /<!-- XML_START -->([\s\S]*?)<!-- XML_END -->/,
  );
  if (commentMatch && commentMatch[1]) {
    return commentMatch[1].trim();
  }

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
export function parseOTCQuoteXML(xmlString: string): OTCQuote | null {
  const parser = new DOMParser();
  const xmlDoc = parser.parseFromString(xmlString, "text/xml");

  // Check for parsing errors
  const parseError = xmlDoc.querySelector("parsererror");
  if (parseError) {
    console.error("XML parsing error:", parseError.textContent);
    return null;
  }

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
export function parseQuoteAcceptedXML(xmlString: string): QuoteAccepted | null {
  const parser = new DOMParser();
  const xmlDoc = parser.parseFromString(xmlString, "text/xml");

  // Check for parsing errors
  const parseError = xmlDoc.querySelector("parsererror");
  if (parseError) {
    console.error("XML parsing error:", parseError.textContent);
    return null;
  }

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
export function messageContainsQuote(messageText: string): boolean {
  return !!(
    messageText.includes("<quote>") ||
    messageText.includes("<quote>") ||
    messageText.includes("<quoteAccepted>") ||
    messageText.includes("<!-- XML_START -->")
  );
}

/**
 * Parse any XML type from message
 */
export function parseMessageXML(messageText: string): {
  type: "otc_quote" | "quote_accepted" | null;
  data: OTCQuote | QuoteAccepted | null;
} {
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
