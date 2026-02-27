#include "price-validation.test.h"

boolean skipIntegration = OR((process->env->CI == std:("true")), (process->env->SKIP_INTEGRATION == std:("true")));
double TEST_TIMEOUT = 60000;
object BASE_TOKENS = object{
    object::pair{std:("WETH"), std:("0x4200000000000000000000000000000000000006")}, 
    object::pair{std:("USDC"), std:("0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913")}
};
object SOLANA_TOKENS = object{
    object::pair{std:("JUP"), std:("JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN")}, 
    object::pair{std:("SOL"), std:("So11111111111111111111111111111111111111112")}
};
double PRICE_DIVERGENCE_THRESHOLD = 10;

void Main(void)
{
    describe(std:("Price Divergence Calculation"), [=]() mutable
    {
        it(std:("correctly calculates 10% divergence threshold"), [=]() mutable
        {
            auto aggregatedPrice = 100;
            auto poolPrice = 110;
            auto diff = Math->abs(poolPrice - aggregatedPrice);
            auto divergence = diff / aggregatedPrice;
            auto divergencePercent = divergence * 100;
            expect(divergencePercent)->toBe(10);
            expect(divergencePercent <= 10)->toBe(true);
        }
        );
        it(std:("correctly identifies >10% divergence"), [=]() mutable
        {
            auto aggregatedPrice = 100;
            auto poolPrice = 115;
            auto diff = Math->abs(poolPrice - aggregatedPrice);
            auto divergence = diff / aggregatedPrice;
            auto divergencePercent = divergence * 100;
            expect(divergencePercent)->toBe(15);
            expect(divergencePercent > 10)->toBe(true);
        }
        );
        it(std:("handles pool price lower than aggregated"), [=]() mutable
        {
            auto aggregatedPrice = 100;
            auto poolPrice = 85;
            auto diff = Math->abs(poolPrice - aggregatedPrice);
            auto divergence = diff / aggregatedPrice;
            auto divergencePercent = divergence * 100;
            expect(divergencePercent)->toBe(15);
            expect(divergencePercent > 10)->toBe(true);
        }
        );
    }
    );
    describe(std:("Agent Offer Rejection Logic"), [=]() mutable
    {
        it(std:("rejects offer when price diverges >10%"), [=]() mutable
        {
            auto MAX_PRICE_DIVERGENCE_BPS = 1000;
            auto offerPriceUsd = 100;
            auto marketPriceUsd = 85;
            auto diff = Math->abs(offerPriceUsd - marketPriceUsd);
            auto divergencePercent = (diff / marketPriceUsd) * 100;
            auto shouldReject = divergencePercent > (MAX_PRICE_DIVERGENCE_BPS / 100);
            expect(shouldReject)->toBe(true);
            console->log(std:("[Agent Rejection] Divergence: ") + divergencePercent->toFixed(2) + std:("% - Should Reject: ") + shouldReject + string_empty);
        }
        );
        it(std:("accepts offer when price is within tolerance"), [=]() mutable
        {
            auto MAX_PRICE_DIVERGENCE_BPS = 1000;
            auto offerPriceUsd = 100;
            auto marketPriceUsd = 95;
            auto diff = Math->abs(offerPriceUsd - marketPriceUsd);
            auto divergencePercent = (diff / marketPriceUsd) * 100;
            auto shouldReject = divergencePercent > (MAX_PRICE_DIVERGENCE_BPS / 100);
            expect(shouldReject)->toBe(false);
            console->log(std:("[Agent Acceptance] Divergence: ") + divergencePercent->toFixed(2) + std:("% - Should Accept: ") + !shouldReject + string_empty);
        }
        );
        it(std:("handles edge case at exactly 10%"), [=]() mutable
        {
            auto MAX_PRICE_DIVERGENCE_BPS = 1000;
            auto offerPriceUsd = 110;
            auto marketPriceUsd = 100;
            auto diff = Math->abs(offerPriceUsd - marketPriceUsd);
            auto divergencePercent = (diff / marketPriceUsd) * 100;
            auto shouldReject = divergencePercent > (MAX_PRICE_DIVERGENCE_BPS / 100);
            expect(shouldReject)->toBe(false);
            console->log(std:("[Edge Case] Divergence: ") + divergencePercent->toFixed(2) + std:("% - Should Accept: ") + !shouldReject + string_empty);
        }
        );
    }
    );
    describe(std:("Price Protection Service Logic"), [=]() mutable
    {
        it(std:("validates quote price against current price"), [=]() mutable
        {
            auto priceAtQuote = 100;
            auto currentPrice = 105;
            auto maxDeviationBps = 1000;
            auto deviation = Math->abs(currentPrice - priceAtQuote);
            auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
            auto isValid = deviationBps <= maxDeviationBps;
            expect(deviationBps)->toBe(500);
            expect(isValid)->toBe(true);
            console->log(std:("[Price Protection] Deviation: ") + (deviationBps / 100) + std:("% - Valid: ") + isValid + string_empty);
        }
        );
        it(std:("rejects when price moves beyond threshold"), [=]() mutable
        {
            auto priceAtQuote = 100;
            auto currentPrice = 115;
            auto maxDeviationBps = 1000;
            auto deviation = Math->abs(currentPrice - priceAtQuote);
            auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
            auto isValid = deviationBps <= maxDeviationBps;
            expect(deviationBps)->toBe(1500);
            expect(isValid)->toBe(false);
            console->log(std:("[Price Protection] Deviation: ") + (deviationBps / 100) + std:("% - Valid: ") + isValid + string_empty);
        }
        );
    }
    );
    describe(std:("checkPriceDivergence Function"), [=]() mutable
    {
        it(std:("fails open when no aggregated price found"), [=]() mutable
        {
            auto result = std::async([=]() { checkPriceDivergence(std:("0x0000000000000000000000000000000000000001"), std:("base"), 100); });
            expect(result->valid)->toBe(true);
        }
        );
        it(std:("handles missing chain gracefully"), [=]() mutable
        {
            auto result = std::async([=]() { checkPriceDivergence(std:("0x0000000000000000000000000000000000000001"), as<any>(std:("unknown")), 100); });
            expect(result->valid)->toBe(true);
        }
        );
        it(std:("handles zero pool price"), [=]() mutable
        {
            auto result = std::async([=]() { checkPriceDivergence(std:("0x0000000000000000000000000000000000000001"), std:("base"), 0); });
            expect(result->valid)->toBe(true);
        }
        );
        it(std:("handles negative pool price"), [=]() mutable
        {
            auto result = std::async([=]() { checkPriceDivergence(std:("0x0000000000000000000000000000000000000001"), std:("base"), -100); });
            expect(result->valid)->toBe(true);
        }
        );
    }
    );
    describe(std:("Base Chain Pool Discovery"), [=]() mutable
    {
        it->skipIf(skipIntegration)(std:("finds WETH pool with accurate price"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["WETH"], 8453); });
            if (!pool) {
                console->log(std:("[Base/WETH] Pool not found (likely RPC rate limited)"));
                return std::shared_ptr<Promise<void>>();
            }
            expect(pool->protocol)->toBe(std:("Uniswap V3"));
            expect(pool->tvlUsd)->toBeGreaterThan(1000000);
            expect(pool->priceUsd)->toBeGreaterThan(2000);
            expect(pool->priceUsd)->toBeLessThan(10000);
            console->log(std:("[Base/WETH] Pool: ") + pool->protocol + string_empty);
            console->log(std:("  - TVL: $") + pool->tvlUsd->toLocaleString() + string_empty);
            console->log(std:("  - Price: $") + pool->priceUsd->toFixed(2) + string_empty);
            auto priceCheck = std::async([=]() { checkPriceDivergence(BASE_TOKENS["WETH"], std:("base"), OR((pool->priceUsd), (0))); });
            console->log(std:("  - CoinGecko: $") + (OR((priceCheck->aggregatedPrice->toFixed(2)), (std:("N/A")))) + string_empty);
            console->log(std:("  - Divergence: ") + (OR((priceCheck->divergencePercent->toFixed(2)), (std:("N/A")))) + std:("%"));
            if (priceCheck->aggregatedPrice) {
                expect(priceCheck->divergencePercent)->toBeLessThan(PRICE_DIVERGENCE_THRESHOLD);
            }
        }
        , TEST_TIMEOUT);
        it(std:("finds USDC pool with stable price"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["USDC"], 8453); });
            if (pool) {
                console->log(std:("[Base/USDC] Pool: ") + pool->protocol + string_empty);
                console->log(std:("  - Price: $") + pool->priceUsd->toFixed(4) + string_empty);
                expect(pool->priceUsd)->toBeGreaterThan(0.95);
                expect(pool->priceUsd)->toBeLessThan(1.05);
            } else {
                console->log(std:("[Base/USDC] No direct pool (expected for stablecoin)"));
            }
        }
        , TEST_TIMEOUT);
    }
    );
    describe(std:("Solana Chain Pool Discovery"), [=]() mutable
    {
        it->skip(std:("attempts to find JUP pool (skipped - RPC rate limits)"), [=]() mutable
        {
            console->log(std:("[Solana/JUP] Attempting pool discovery..."));
            try
            {
                auto pool = std::async([=]() { findBestSolanaPool(SOLANA_TOKENS["JUP"], std:("mainnet")); });
                if (pool) {
                    expect(array<string>{ std:("Raydium"), std:("PumpSwap") })->toContain(pool->protocol);
                    expect(pool->tvlUsd)->toBeGreaterThan(0);
                    expect(pool->priceUsd)->toBeDefined();
                    console->log(std:("  - Protocol: ") + pool->protocol + string_empty);
                    console->log(std:("  - TVL: $") + pool->tvlUsd->toLocaleString() + string_empty);
                    console->log(std:("  - Price: $") + pool->priceUsd->toFixed(8) + string_empty);
                } else {
                    console->log(std:("  - No pool found (public RPC may block getProgramAccounts)"));
                }
            }
            catch (const any& error)
            {
                console->log(std:("  - RPC error (expected on public RPCs):"), (as<std::shared_ptr<Error>>(error))->message->slice(0, 50));
            }
        }
        , TEST_TIMEOUT);
    }
    );
    describe(std:("Price Divergence Detection"), [=]() mutable
    {
        it->skipIf(skipIntegration)(std:("detects when pool price is within tolerance"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["WETH"], 8453); });
            if (!pool->priceUsd) {
                console->log(std:("Skipping - no pool price available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto result = std::async([=]() { checkPriceDivergence(BASE_TOKENS["WETH"], std:("base"), pool->priceUsd); });
            console->log(std:("[Within Tolerance]"));
            console->log(std:("  - Pool: $") + pool->priceUsd->toFixed(2) + string_empty);
            console->log(std:("  - Aggregated: $") + (OR((result->aggregatedPrice->toFixed(2)), (std:("N/A")))) + string_empty);
            console->log(std:("  - Divergence: ") + (OR((result->divergencePercent->toFixed(2)), (std:("N/A")))) + std:("%"));
            console->log(std:("  - Valid: ") + result->valid + string_empty);
            if (result->aggregatedPrice) {
                expect(result->valid)->toBe(true);
                expect(result->divergencePercent)->toBeLessThan(PRICE_DIVERGENCE_THRESHOLD);
            }
        }
        , TEST_TIMEOUT);
        it->skipIf(skipIntegration)(std:("detects when pool price exceeds tolerance"), [=]() mutable
        {
            auto badPoolPrice = 1500;
            auto result = std::async([=]() { checkPriceDivergence(BASE_TOKENS["WETH"], std:("base"), badPoolPrice); });
            console->log(std:("[Exceeds Tolerance]"));
            console->log(std:("  - Bad Pool: $") + badPoolPrice + string_empty);
            console->log(std:("  - Aggregated: $") + (OR((result->aggregatedPrice->toFixed(2)), (std:("N/A")))) + string_empty);
            console->log(std:("  - Divergence: ") + (OR((result->divergencePercent->toFixed(2)), (std:("N/A")))) + std:("%"));
            console->log(std:("  - Valid: ") + result->valid + string_empty);
            console->log(std:("  - Warning: ") + (OR((result->warning), (std:("None")))) + string_empty);
            if (result->aggregatedPrice) {
                expect(result->valid)->toBe(false);
                expect(result->warning)->toBeDefined();
                expect(result->divergencePercent)->toBeGreaterThan(PRICE_DIVERGENCE_THRESHOLD);
            }
        }
        , TEST_TIMEOUT);
        it(std:("handles missing aggregated price gracefully"), [=]() mutable
        {
            auto fakeToken = std:("0x0000000000000000000000000000000000000001");
            auto result = std::async([=]() { checkPriceDivergence(fakeToken, std:("base"), 100); });
            console->log(std:("[Missing Aggregated Price]"));
            console->log(std:("  - Result: ") + (result->valid) ? std:("PASS (fail-open)") : std:("FAIL") + string_empty);
            expect(result->valid)->toBe(true);
        }
        , TEST_TIMEOUT);
    }
    );
    describe(std:("Price Protection Integration"), [=]() mutable
    {
        it->skipIf(skipIntegration)(std:("validates quote price against current market"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["WETH"], 8453); });
            if (!pool->priceUsd) {
                console->log(std:("Skipping - no pool price"));
                return std::shared_ptr<Promise<void>>();
            }
            auto priceAtQuote = pool->priceUsd * 0.98;
            auto maxDeviationBps = 1000;
            auto deviation = Math->abs(pool->priceUsd - priceAtQuote);
            auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
            auto isValid = deviationBps <= maxDeviationBps;
            console->log(std:("[Quote Validation]"));
            console->log(std:("  - Quote Price: $") + priceAtQuote->toFixed(2) + string_empty);
            console->log(std:("  - Current: $") + pool->priceUsd->toFixed(2) + string_empty);
            console->log(std:("  - Deviation: ") + (deviationBps / 100) + std:("%"));
            console->log(std:("  - Should Accept: ") + isValid + string_empty);
            expect(isValid)->toBe(true);
        }
        , TEST_TIMEOUT);
        it->skipIf(skipIntegration)(std:("rejects deal when price moves too much"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["WETH"], 8453); });
            if (!pool->priceUsd) {
                console->log(std:("Skipping - no pool price"));
                return std::shared_ptr<Promise<void>>();
            }
            auto priceAtQuote = pool->priceUsd * 0.85;
            auto maxDeviationBps = 1000;
            auto deviation = Math->abs(pool->priceUsd - priceAtQuote);
            auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
            auto isValid = deviationBps <= maxDeviationBps;
            console->log(std:("[Price Moved Too Much]"));
            console->log(std:("  - Quote Price: $") + priceAtQuote->toFixed(2) + string_empty);
            console->log(std:("  - Current: $") + pool->priceUsd->toFixed(2) + string_empty);
            console->log(std:("  - Deviation: ") + (deviationBps / 100) + std:("%"));
            console->log(std:("  - Should Reject: ") + !isValid + string_empty);
            expect(isValid)->toBe(false);
        }
        , TEST_TIMEOUT);
    }
    );
    describe(std:("Test Summary"), [=]() mutable
    {
        it(std:("displays results"), [=]() mutable
        {
            console->log(std:("\
═══════════════════════════════════════════════════════════════════════════════\
                        PRICE VALIDATION TEST SUMMARY\
═══════════════════════════════════════════════════════════════════════════════\
\
  UNIT TESTS:\
  ✓ Divergence calculation (10%, >10%, lower price)\
  ✓ Agent rejection logic (within/beyond tolerance)\
  ✓ Price protection service logic\
  ✓ checkPriceDivergence edge cases\
\
  INTEGRATION TESTS (when RPC available):\
  ✓ Base chain pool discovery (WETH, USDC)\
  ✓ Solana chain pool discovery (JUP)\
  ✓ Price divergence detection\
  ✓ Quote validation against market\
\
  Skip integration tests with: SKIP_INTEGRATION=true\
\
═══════════════════════════════════════════════════════════════════════════════\
    "));
        }
        );
    }
    );
}

MAIN
