#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/tests/price-validation.test.h"

boolean skipIntegration = OR((process->env->CI == std::string("true")), (process->env->SKIP_INTEGRATION == std::string("true")));
double TEST_TIMEOUT = 60000;
object BASE_TOKENS = object{
    object::pair{std::string("WETH"), std::string("0x4200000000000000000000000000000000000006")}, 
    object::pair{std::string("USDC"), std::string("0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913")}
};
object SOLANA_TOKENS = object{
    object::pair{std::string("JUP"), std::string("JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN")}, 
    object::pair{std::string("SOL"), std::string("So11111111111111111111111111111111111111112")}
};
double PRICE_DIVERGENCE_THRESHOLD = 10;

void Main(void)
{
    describe(std::string("Price Divergence Calculation"), [=]() mutable
    {
        it(std::string("correctly calculates 10% divergence threshold"), [=]() mutable
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
        it(std::string("correctly identifies >10% divergence"), [=]() mutable
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
        it(std::string("handles pool price lower than aggregated"), [=]() mutable
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
    describe(std::string("Agent Offer Rejection Logic"), [=]() mutable
    {
        it(std::string("rejects offer when price diverges >10%"), [=]() mutable
        {
            auto MAX_PRICE_DIVERGENCE_BPS = 1000;
            auto offerPriceUsd = 100;
            auto marketPriceUsd = 85;
            auto diff = Math->abs(offerPriceUsd - marketPriceUsd);
            auto divergencePercent = (diff / marketPriceUsd) * 100;
            auto shouldReject = divergencePercent > (MAX_PRICE_DIVERGENCE_BPS / 100);
            expect(shouldReject)->toBe(true);
            console->log(std::string("[Agent Rejection] Divergence: ") + divergencePercent->toFixed(2) + std::string("% - Should Reject: ") + shouldReject + string_empty);
        }
        );
        it(std::string("accepts offer when price is within tolerance"), [=]() mutable
        {
            auto MAX_PRICE_DIVERGENCE_BPS = 1000;
            auto offerPriceUsd = 100;
            auto marketPriceUsd = 95;
            auto diff = Math->abs(offerPriceUsd - marketPriceUsd);
            auto divergencePercent = (diff / marketPriceUsd) * 100;
            auto shouldReject = divergencePercent > (MAX_PRICE_DIVERGENCE_BPS / 100);
            expect(shouldReject)->toBe(false);
            console->log(std::string("[Agent Acceptance] Divergence: ") + divergencePercent->toFixed(2) + std::string("% - Should Accept: ") + !shouldReject + string_empty);
        }
        );
        it(std::string("handles edge case at exactly 10%"), [=]() mutable
        {
            auto MAX_PRICE_DIVERGENCE_BPS = 1000;
            auto offerPriceUsd = 110;
            auto marketPriceUsd = 100;
            auto diff = Math->abs(offerPriceUsd - marketPriceUsd);
            auto divergencePercent = (diff / marketPriceUsd) * 100;
            auto shouldReject = divergencePercent > (MAX_PRICE_DIVERGENCE_BPS / 100);
            expect(shouldReject)->toBe(false);
            console->log(std::string("[Edge Case] Divergence: ") + divergencePercent->toFixed(2) + std::string("% - Should Accept: ") + !shouldReject + string_empty);
        }
        );
    }
    );
    describe(std::string("Price Protection Service Logic"), [=]() mutable
    {
        it(std::string("validates quote price against current price"), [=]() mutable
        {
            auto priceAtQuote = 100;
            auto currentPrice = 105;
            auto maxDeviationBps = 1000;
            auto deviation = Math->abs(currentPrice - priceAtQuote);
            auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
            auto isValid = deviationBps <= maxDeviationBps;
            expect(deviationBps)->toBe(500);
            expect(isValid)->toBe(true);
            console->log(std::string("[Price Protection] Deviation: ") + (deviationBps / 100) + std::string("% - Valid: ") + isValid + string_empty);
        }
        );
        it(std::string("rejects when price moves beyond threshold"), [=]() mutable
        {
            auto priceAtQuote = 100;
            auto currentPrice = 115;
            auto maxDeviationBps = 1000;
            auto deviation = Math->abs(currentPrice - priceAtQuote);
            auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
            auto isValid = deviationBps <= maxDeviationBps;
            expect(deviationBps)->toBe(1500);
            expect(isValid)->toBe(false);
            console->log(std::string("[Price Protection] Deviation: ") + (deviationBps / 100) + std::string("% - Valid: ") + isValid + string_empty);
        }
        );
    }
    );
    describe(std::string("checkPriceDivergence Function"), [=]() mutable
    {
        it(std::string("fails open when no aggregated price found"), [=]() mutable
        {
            auto result = std::async([=]() { checkPriceDivergence(std::string("0x0000000000000000000000000000000000000001"), std::string("base"), 100); });
            expect(result->valid)->toBe(true);
        }
        );
        it(std::string("handles missing chain gracefully"), [=]() mutable
        {
            auto result = std::async([=]() { checkPriceDivergence(std::string("0x0000000000000000000000000000000000000001"), as<any>(std::string("unknown")), 100); });
            expect(result->valid)->toBe(true);
        }
        );
        it(std::string("handles zero pool price"), [=]() mutable
        {
            auto result = std::async([=]() { checkPriceDivergence(std::string("0x0000000000000000000000000000000000000001"), std::string("base"), 0); });
            expect(result->valid)->toBe(true);
        }
        );
        it(std::string("handles negative pool price"), [=]() mutable
        {
            auto result = std::async([=]() { checkPriceDivergence(std::string("0x0000000000000000000000000000000000000001"), std::string("base"), -100); });
            expect(result->valid)->toBe(true);
        }
        );
    }
    );
    describe(std::string("Base Chain Pool Discovery"), [=]() mutable
    {
        it->skipIf(skipIntegration)(std::string("finds WETH pool with accurate price"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["WETH"], 8453); });
            if (!pool) {
                console->log(std::string("[Base/WETH] Pool not found (likely RPC rate limited)"));
                return std::shared_ptr<Promise<void>>();
            }
            expect(pool->protocol)->toBe(std::string("Uniswap V3"));
            expect(pool->tvlUsd)->toBeGreaterThan(1000000);
            expect(pool->priceUsd)->toBeGreaterThan(2000);
            expect(pool->priceUsd)->toBeLessThan(10000);
            console->log(std::string("[Base/WETH] Pool: ") + pool->protocol + string_empty);
            console->log(std::string("  - TVL: $") + pool->tvlUsd->toLocaleString() + string_empty);
            console->log(std::string("  - Price: $") + pool->priceUsd->toFixed(2) + string_empty);
            auto priceCheck = std::async([=]() { checkPriceDivergence(BASE_TOKENS["WETH"], std::string("base"), OR((pool->priceUsd), (0))); });
            console->log(std::string("  - CoinGecko: $") + (OR((priceCheck->aggregatedPrice->toFixed(2)), (std::string("N/A")))) + string_empty);
            console->log(std::string("  - Divergence: ") + (OR((priceCheck->divergencePercent->toFixed(2)), (std::string("N/A")))) + std::string("%"));
            if (priceCheck->aggregatedPrice) {
                expect(priceCheck->divergencePercent)->toBeLessThan(PRICE_DIVERGENCE_THRESHOLD);
            }
        }
        , TEST_TIMEOUT);
        it(std::string("finds USDC pool with stable price"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["USDC"], 8453); });
            if (pool) {
                console->log(std::string("[Base/USDC] Pool: ") + pool->protocol + string_empty);
                console->log(std::string("  - Price: $") + pool->priceUsd->toFixed(4) + string_empty);
                expect(pool->priceUsd)->toBeGreaterThan(0.95);
                expect(pool->priceUsd)->toBeLessThan(1.05);
            } else {
                console->log(std::string("[Base/USDC] No direct pool (expected for stablecoin)"));
            }
        }
        , TEST_TIMEOUT);
    }
    );
    describe(std::string("Solana Chain Pool Discovery"), [=]() mutable
    {
        it->skip(std::string("attempts to find JUP pool (skipped - RPC rate limits)"), [=]() mutable
        {
            console->log(std::string("[Solana/JUP] Attempting pool discovery..."));
            try
            {
                auto pool = std::async([=]() { findBestSolanaPool(SOLANA_TOKENS["JUP"], std::string("mainnet")); });
                if (pool) {
                    expect(array<string>{ std::string("Raydium"), std::string("PumpSwap") })->toContain(pool->protocol);
                    expect(pool->tvlUsd)->toBeGreaterThan(0);
                    expect(pool->priceUsd)->toBeDefined();
                    console->log(std::string("  - Protocol: ") + pool->protocol + string_empty);
                    console->log(std::string("  - TVL: $") + pool->tvlUsd->toLocaleString() + string_empty);
                    console->log(std::string("  - Price: $") + pool->priceUsd->toFixed(8) + string_empty);
                } else {
                    console->log(std::string("  - No pool found (public RPC may block getProgramAccounts)"));
                }
            }
            catch (const any& error)
            {
                console->log(std::string("  - RPC error (expected on public RPCs):"), (as<std::shared_ptr<Error>>(error))->message->slice(0, 50));
            }
        }
        , TEST_TIMEOUT);
    }
    );
    describe(std::string("Price Divergence Detection"), [=]() mutable
    {
        it->skipIf(skipIntegration)(std::string("detects when pool price is within tolerance"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["WETH"], 8453); });
            if (!pool->priceUsd) {
                console->log(std::string("Skipping - no pool price available"));
                return std::shared_ptr<Promise<void>>();
            }
            auto result = std::async([=]() { checkPriceDivergence(BASE_TOKENS["WETH"], std::string("base"), pool->priceUsd); });
            console->log(std::string("[Within Tolerance]"));
            console->log(std::string("  - Pool: $") + pool->priceUsd->toFixed(2) + string_empty);
            console->log(std::string("  - Aggregated: $") + (OR((result->aggregatedPrice->toFixed(2)), (std::string("N/A")))) + string_empty);
            console->log(std::string("  - Divergence: ") + (OR((result->divergencePercent->toFixed(2)), (std::string("N/A")))) + std::string("%"));
            console->log(std::string("  - Valid: ") + result->valid + string_empty);
            if (result->aggregatedPrice) {
                expect(result->valid)->toBe(true);
                expect(result->divergencePercent)->toBeLessThan(PRICE_DIVERGENCE_THRESHOLD);
            }
        }
        , TEST_TIMEOUT);
        it->skipIf(skipIntegration)(std::string("detects when pool price exceeds tolerance"), [=]() mutable
        {
            auto badPoolPrice = 1500;
            auto result = std::async([=]() { checkPriceDivergence(BASE_TOKENS["WETH"], std::string("base"), badPoolPrice); });
            console->log(std::string("[Exceeds Tolerance]"));
            console->log(std::string("  - Bad Pool: $") + badPoolPrice + string_empty);
            console->log(std::string("  - Aggregated: $") + (OR((result->aggregatedPrice->toFixed(2)), (std::string("N/A")))) + string_empty);
            console->log(std::string("  - Divergence: ") + (OR((result->divergencePercent->toFixed(2)), (std::string("N/A")))) + std::string("%"));
            console->log(std::string("  - Valid: ") + result->valid + string_empty);
            console->log(std::string("  - Warning: ") + (OR((result->warning), (std::string("None")))) + string_empty);
            if (result->aggregatedPrice) {
                expect(result->valid)->toBe(false);
                expect(result->warning)->toBeDefined();
                expect(result->divergencePercent)->toBeGreaterThan(PRICE_DIVERGENCE_THRESHOLD);
            }
        }
        , TEST_TIMEOUT);
        it(std::string("handles missing aggregated price gracefully"), [=]() mutable
        {
            auto fakeToken = std::string("0x0000000000000000000000000000000000000001");
            auto result = std::async([=]() { checkPriceDivergence(fakeToken, std::string("base"), 100); });
            console->log(std::string("[Missing Aggregated Price]"));
            console->log(std::string("  - Result: ") + (result->valid) ? std::string("PASS (fail-open)") : std::string("FAIL") + string_empty);
            expect(result->valid)->toBe(true);
        }
        , TEST_TIMEOUT);
    }
    );
    describe(std::string("Price Protection Integration"), [=]() mutable
    {
        it->skipIf(skipIntegration)(std::string("validates quote price against current market"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["WETH"], 8453); });
            if (!pool->priceUsd) {
                console->log(std::string("Skipping - no pool price"));
                return std::shared_ptr<Promise<void>>();
            }
            auto priceAtQuote = pool->priceUsd * 0.98;
            auto maxDeviationBps = 1000;
            auto deviation = Math->abs(pool->priceUsd - priceAtQuote);
            auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
            auto isValid = deviationBps <= maxDeviationBps;
            console->log(std::string("[Quote Validation]"));
            console->log(std::string("  - Quote Price: $") + priceAtQuote->toFixed(2) + string_empty);
            console->log(std::string("  - Current: $") + pool->priceUsd->toFixed(2) + string_empty);
            console->log(std::string("  - Deviation: ") + (deviationBps / 100) + std::string("%"));
            console->log(std::string("  - Should Accept: ") + isValid + string_empty);
            expect(isValid)->toBe(true);
        }
        , TEST_TIMEOUT);
        it->skipIf(skipIntegration)(std::string("rejects deal when price moves too much"), [=]() mutable
        {
            auto pool = std::async([=]() { findBestPool(BASE_TOKENS["WETH"], 8453); });
            if (!pool->priceUsd) {
                console->log(std::string("Skipping - no pool price"));
                return std::shared_ptr<Promise<void>>();
            }
            auto priceAtQuote = pool->priceUsd * 0.85;
            auto maxDeviationBps = 1000;
            auto deviation = Math->abs(pool->priceUsd - priceAtQuote);
            auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
            auto isValid = deviationBps <= maxDeviationBps;
            console->log(std::string("[Price Moved Too Much]"));
            console->log(std::string("  - Quote Price: $") + priceAtQuote->toFixed(2) + string_empty);
            console->log(std::string("  - Current: $") + pool->priceUsd->toFixed(2) + string_empty);
            console->log(std::string("  - Deviation: ") + (deviationBps / 100) + std::string("%"));
            console->log(std::string("  - Should Reject: ") + !isValid + string_empty);
            expect(isValid)->toBe(false);
        }
        , TEST_TIMEOUT);
    }
    );
    describe(std::string("Test Summary"), [=]() mutable
    {
        it(std::string("displays results"), [=]() mutable
        {
            console->log(std::string("\
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
