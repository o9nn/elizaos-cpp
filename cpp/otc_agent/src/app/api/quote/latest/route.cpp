#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto runtime = agentRuntime.getRuntime();
    const auto quoteService = runtime.getService<QuoteService>("QuoteService");

    if (!quoteService) {
        return NextResponse.json(;
        { error: "QuoteService not available" },
        { status: 500 },
        );
    }

    const auto { searchParams } = new URL(request.url);
    const auto wallet = searchParams.get("entityId");
    const auto tokenId = searchParams.get("tokenId");

    if (!wallet) {
        return NextResponse.json(;
        { error: "entityId parameter required" },
        { status: 400 },
        );
    }

    if (!tokenId) {
        return NextResponse.json(;
        { error: "tokenId parameter required" },
        { status: 400 },
        );
    }

    std::cout << "[Quote API] GET - wallet:" << wallet << "tokenId:" << tokenId << std::endl;

    const auto entityId = walletToEntityId(wallet);
    const auto quoteKey = "quote:" + tokenId + ":" + entityId;
    auto quote = runtime.getCache<any>(quoteKey);
    std::cout << "[Quote API] Found:" << quote ? quote.quoteId : "nullptr" << std::endl;

    if (!quote) {
        std::cout << "[Quote API] Creating default quote for token:" << tokenId << std::endl;

        // Worst possible deal defaults (lowest discount, longest lockup)
        const auto DEFAULT_MIN_DISCOUNT_BPS = 100; // 1%;
        const auto DEFAULT_MAX_LOCKUP_MONTHS = 12; // 12 months;

        quoteService.createQuote({
            entityId,
            beneficiary: wallet.toLowerCase(),
            tokenAmount: "0",
            discountBps: DEFAULT_MIN_DISCOUNT_BPS,
            apr: 0,
            lockupMonths: DEFAULT_MAX_LOCKUP_MONTHS,
            paymentCurrency: "USDC",
            totalUsd: 0,
            discountUsd: 0,
            discountedUsd: 0,
            paymentAmount: "0",
            });

            quote = runtime.getCache<any>(quoteKey);
            if (quote) {
                quote.tokenId = tokenId;
                runtime.setCache(quoteKey, quote);
            }
        }

        // Fetch token data to get chain and symbol information
        auto tokenChain = std::nullopt;
        auto tokenSymbol = std::nullopt;
        if (tokenId) {
            const auto { TokenDB } = import("@/services/database");
            const auto token = TokenDB.getToken(tokenId);
            tokenChain = token.chain;
            tokenSymbol = token.symbol;
        }

        const auto formattedQuote = quote;
        ? {
            quoteId: quote.quoteId,
            entityId: quote.entityId,
            beneficiary: quote.beneficiary,
            tokenId: quote.tokenId || tokenId,
            tokenAmount: quote.tokenAmount,
            discountBps: quote.discountBps,
            apr: quote.apr,
            lockupMonths: quote.lockupMonths,
            lockupDays: quote.lockupDays,
            paymentCurrency: quote.paymentCurrency,
            totalUsd: quote.totalUsd,
            discountUsd: quote.discountUsd,
            discountedUsd: quote.discountedUsd,
            paymentAmount: quote.paymentAmount,
            status: quote.status,
            createdAt: quote.createdAt,
            tokenChain,
            tokenSymbol,
        }
        : nullptr;

        console.log(
        "[Quote API] Returning:",
        formattedQuote.quoteId || "nullptr",
        "chain:",
        tokenChain,
        );
        return NextResponse.json({ success: true, quote: formattedQuote });

}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto runtime = agentRuntime.getRuntime();
    const auto quoteService = runtime.getService<QuoteService>("QuoteService");

    if (!quoteService) {
        return NextResponse.json(;
        { error: "QuoteService not available" },
        { status: 500 },
        );
    }

    const auto body = request.json();
    const auto {;
        quoteId,
        beneficiary,
        tokenAmount,
        paymentCurrency,
        totalUsd,
        discountUsd,
        discountedUsd,
        paymentAmount,
        } = body;

        if (!quoteId) {
            return NextResponse.json({ error: "quoteId required" }, { status: 400 });
        }

        console.log("[Quote API] POST - updating quote:", {
            quoteId,
            beneficiary: beneficiary.slice(0, 10),
            tokenAmount,
            paymentCurrency,
            totalUsd,
            discountedUsd,
            });

            // Get existing quote
            const auto quote = quoteService.getQuoteByQuoteId(quoteId);

            if (!quote) {
                std::cerr << "[Quote API] Quote not found:" << quoteId << std::endl;
                return NextResponse.json({ error: "Quote not found" }, { status: 404 });
            }

            // Update all provided fields
            const auto updatedQuote = {;
                ...quote,
                ...(beneficiary && { beneficiary: beneficiary.toLowerCase() }),
                ...(tokenAmount && { tokenAmount: std::to_string(tokenAmount) }),
                ...(paymentCurrency && { paymentCurrency }),
                ...(typeof totalUsd == "number" && { totalUsd }),
                ...(typeof discountUsd == "number" && { discountUsd }),
                ...(typeof discountedUsd == "number" && { discountedUsd }),
                ...(paymentAmount && { paymentAmount: std::to_string(paymentAmount) }),
                };

                // Save updated quote
                "runtime.setCache(" + "quote:" + quoteId

                std::cout << "[Quote API] ✅ Quote updated:" << quoteId << std::endl;

                return NextResponse.json({ success: true, quote: updatedQuote });

}

} // namespace elizaos
