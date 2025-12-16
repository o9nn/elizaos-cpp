#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

async : { params: Promise<{ offerId: string }> },
) {
  await agentRuntime.getRuntime();

  const { offerId } = await params;

  if (!offerId) {
    return NextResponse.json({ error: "Offer ID required" }, { status: 400 });
  }

  const runtime = await agentRuntime.getRuntime();
  const quoteService = runtime.getService<QuoteService>("QuoteService");

  if (!quoteService) {
    return NextResponse.json(
      { error: "QuoteService not available" },
      { status: 500 },
    );
  }

  // Search for quote with matching offerId
  const matchingQuote = await quoteService.getQuoteByOfferId(offerId);

  if (!matchingQuote) {
    return NextResponse.json(
      { error: "Quote not found for this offer" },
      { status: 404 },
    );
  }

  // Redirect to the deal page
  return NextResponse.redirect(
    new URL(`/deal/${matchingQuote.quoteId}`, request.url),
  );
}

} // namespace elizaos
