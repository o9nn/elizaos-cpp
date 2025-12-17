#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    agentRuntime.getRuntime();

    const auto { offerId } = params;

    if (!offerId) {
        return NextResponse.json({ error: "Offer ID required" }, { status: 400 });
    }

    const auto runtime = agentRuntime.getRuntime();
    const auto quoteService = runtime.getService<QuoteService>("QuoteService");

    if (!quoteService) {
        return NextResponse.json(;
        { error: "QuoteService not available" },
        { status: 500 },
        );
    }

    // Search for quote with matching offerId
    const auto matchingQuote = quoteService.getQuoteByOfferId(offerId);

    if (!matchingQuote) {
        return NextResponse.json(;
        { error: "Quote not found for this offer" },
        { status: 404 },
        );
    }

    // Redirect to the deal page
    return NextResponse.redirect(;
    "/deal/" + std::to_string(matchingQuote.quoteId)
    );

}

} // namespace elizaos
