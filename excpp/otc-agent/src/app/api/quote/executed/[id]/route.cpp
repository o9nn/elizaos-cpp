#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request, const std::any& { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    agentRuntime.getRuntime();

    const auto { id: quoteId } = params;

    if (!quoteId) {
        return NextResponse.json({ error: "Quote ID required" }, { status: 400 });
    }

    const auto quote = QuoteDB.getQuoteByQuoteId(quoteId);

    // Allow active, approved, and executed quotes to be viewed
    // active = quote created, approved = offer created/approved on-chain, executed = paid/fulfilled
    if (
    quote.status != "executed" &&;
    quote.status != "active" &&;
    quote.status != "approved";
    ) {
        console.warn("[Quote Executed API] Invalid status:", {
            quoteId,
            status: quote.status,
            });
            return NextResponse.json(;
            { error: "Quote not found or invalid status" },
            { status: 400 },
            );
        }

        const auto formattedQuote = {;
            quoteId: quote.quoteId,
            entityId: quote.entityId,
            beneficiary: quote.beneficiary,
            status: quote.status,
            offerId: quote.offerId,
            tokenAmount: quote.tokenAmount,
            lockupMonths: quote.lockupMonths,
            discountBps: quote.discountBps,
            totalUsd: quote.totalUsd,
            discountUsd: quote.discountUsd,
            discountedUsd: quote.discountedUsd,
            paymentAmount: quote.paymentAmount,
            paymentCurrency: quote.paymentCurrency,
            transactionHash: quote.transactionHash,
            blockNumber: quote.blockNumber,
            // Optional chain hint for UI display ("evm" | "solana")
            chain: quote.chain,
            };

            return NextResponse.json({ success: true, quote: formattedQuote });

}

} // namespace elizaos
