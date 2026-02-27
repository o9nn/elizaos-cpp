#include "service.test.h"

void Main(void)
{
    describe(std:("DummyLpService"), [=]() mutable
    {
        shared<std::shared_ptr<DummyLpService>> service;
        shared mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
        beforeEach([=]() mutable
        {
            service = std::make_shared<DummyLpService>();
            service->start(mockRuntime);
        }
        );
        it(std:("should have the correct DEX name"), [=]() mutable
        {
            expect(service->getDexName())->toBe(std:("dummy"));
        }
        );
        describe(std:("getPools"), [=]() mutable
        {
            it(std:("should return all dummy pools when no mints are specified"), [=]() mutable
            {
                auto pools = std::async([=]() { service->getPools(); });
                expect(pools->get_length())->toBe(2);
                expect(const_(pools)[0]->id)->toBe(std:("dummy-pool-1"));
                expect(const_(pools)[1]->id)->toBe(std:("dummy-stable-pool-2"));
            }
            );
            it(std:("should filter pools by tokenA mint"), [=]() mutable
            {
                auto SOL_MINT = std:("So11111111111111111111111111111111111111112");
                auto pools = std::async([=]() { service->getPools(SOL_MINT); });
                expect(pools->get_length())->toBe(1);
                expect(const_(pools)[0]->id)->toBe(std:("dummy-pool-1"));
            }
            );
            it(std:("should return an empty array if no pools match the filter"), [=]() mutable
            {
                auto pools = std::async([=]() { service->getPools(std:("non-existent-mint")); });
                expect(pools->get_length())->toBe(0);
            }
            );
        }
        );
        describe(std:("addLiquidity"), [=]() mutable
        {
            it(std:("should return a successful transaction result with LP tokens"), [=]() mutable
            {
                auto result = std::async([=]() { service->addLiquidity(object{
                    object::pair{std:("userVault"), Keypair->generate()}, 
                    object::pair{std:("poolId"), std:("dummy-pool-1")}, 
                    object::pair{std:("tokenAAmountLamports"), std:("1000000000")}, 
                    object::pair{std:("slippageBps"), 50}
                }); });
                expect(result["success"])->toBe(true);
                expect(result["transactionId"])->toMatch((new RegExp(std:("^dummy-tx-\d+"))));
                expect(result["lpTokensReceived"])->toBeDefined();
                expect(result["lpTokensReceived"]["address"])->toBe(std:("dummy-lp-mint-dummy-pool-1"));
                expect(result["lpTokensReceived"]["uiAmount"])->toBe(100);
            }
            );
        }
        );
        describe(std:("removeLiquidity"), [=]() mutable
        {
            it(std:("should return a successful transaction result with underlying tokens"), [=]() mutable
            {
                auto result = std::async([=]() { service->removeLiquidity(object{
                    object::pair{std:("userVault"), Keypair->generate()}, 
                    object::pair{std:("poolId"), std:("dummy-pool-1")}, 
                    object::pair{std:("lpTokenAmountLamports"), std:("100000000")}, 
                    object::pair{std:("slippageBps"), 50}
                }); });
                expect(result["success"])->toBe(true);
                expect(result["transactionId"])->toMatch((new RegExp(std:("^dummy-tx-\d+"))));
                expect(result["tokensReceived"])->toBeDefined();
                expect(result["tokensReceived"]["length"])->toBe(2);
                expect(const_(result["tokensReceived"])[0]["symbol"])->toBe(std:("SOL"));
                expect(const_(result["tokensReceived"])[1]["symbol"])->toBe(std:("USDC"));
            }
            );
        }
        );
        describe(std:("getLpPositionDetails"), [=]() mutable
        {
            it(std:("should return mock LP position details"), [=]() mutable
            {
                auto userPublicKey = Keypair->generate()->publicKey->toBase58();
                auto positionId = std:("dummy-lp-mint-dummy-pool-1");
                auto details = std::async([=]() { service->getLpPositionDetails(userPublicKey, positionId); });
                expect(details)->toBeDefined();
                expect(details["dex"])->toBe(std:("dummy"));
                expect(details["poolId"])->toBe(std:("dummy-pool-1"));
                expect(details["valueUsd"])->toBe(1000);
                expect(details["underlyingTokens"]["length"])->toBe(2);
                expect(details["lpTokenBalance"]["address"])->toBe(positionId);
            }
            );
        }
        );
        describe(std:("getMarketDataForPools"), [=]() mutable
        {
            it(std:("should return random market data for given pool IDs"), [=]() mutable
            {
                auto poolIds = array<string>{ std:("dummy-pool-1"), std:("dummy-stable-pool-2") };
                auto marketData = std::async([=]() { service->getMarketDataForPools(poolIds); });
                expect(Object->keys(marketData)->get_length())->toBe(2);
                expect((*const_(marketData))[std:("dummy-pool-1")])->toBeDefined();
                expect((*const_(marketData))[std:("dummy-pool-1")]->apy)->toBeTypeOf(std:("number"));
                expect((*const_(marketData))[std:("dummy-pool-1")]->tvl)->toBeTypeOf(std:("number"));
                expect((*const_(marketData))[std:("dummy-stable-pool-2")])->toBeDefined();
            }
            );
        }
        );
    }
    );
}

MAIN
