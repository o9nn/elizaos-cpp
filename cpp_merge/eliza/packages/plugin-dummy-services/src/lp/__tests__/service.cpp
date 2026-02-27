#include "service.test.h"
#include <string>

void Main(void)
{
    describe(std::string("DummyLpService"), [=]() mutable
    {
        shared<std::shared_ptr<DummyLpService>> service;
        shared mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
        beforeEach([=]() mutable
        {
            service = std::make_shared<DummyLpService>();
            service->start(mockRuntime);
        }
        );
        it(std::string("should have the correct DEX name"), [=]() mutable
        {
            expect(service->getDexName())->toBe(std::string("dummy"));
        }
        );
        describe(std::string("getPools"), [=]() mutable
        {
            it(std::string("should return all dummy pools when no mints are specified"), [=]() mutable
            {
                auto pools = std::async([=]() { service->getPools(); });
                expect(pools->get_length())->toBe(2);
                expect(const_(pools)[0]->id)->toBe(std::string("dummy-pool-1"));
                expect(const_(pools)[1]->id)->toBe(std::string("dummy-stable-pool-2"));
            }
            );
            it(std::string("should filter pools by tokenA mint"), [=]() mutable
            {
                auto SOL_MINT = std::string("So11111111111111111111111111111111111111112");
                auto pools = std::async([=]() { service->getPools(SOL_MINT); });
                expect(pools->get_length())->toBe(1);
                expect(const_(pools)[0]->id)->toBe(std::string("dummy-pool-1"));
            }
            );
            it(std::string("should return an empty array if no pools match the filter"), [=]() mutable
            {
                auto pools = std::async([=]() { service->getPools(std::string("non-existent-mint")); });
                expect(pools->get_length())->toBe(0);
            }
            );
        }
        );
        describe(std::string("addLiquidity"), [=]() mutable
        {
            it(std::string("should return a successful transaction result with LP tokens"), [=]() mutable
            {
                auto result = std::async([=]() { service->addLiquidity(object{
                    object::pair{std::string("userVault"), Keypair->generate()}, 
                    object::pair{std::string("poolId"), std::string("dummy-pool-1")}, 
                    object::pair{std::string("tokenAAmountLamports"), std::string("1000000000")}, 
                    object::pair{std::string("slippageBps"), 50}
                }); });
                expect(result["success"])->toBe(true);
                expect(result["transactionId"])->toMatch((new RegExp(std::string("^dummy-tx-\d+"))));
                expect(result["lpTokensReceived"])->toBeDefined();
                expect(result["lpTokensReceived"]["address"])->toBe(std::string("dummy-lp-mint-dummy-pool-1"));
                expect(result["lpTokensReceived"]["uiAmount"])->toBe(100);
            }
            );
        }
        );
        describe(std::string("removeLiquidity"), [=]() mutable
        {
            it(std::string("should return a successful transaction result with underlying tokens"), [=]() mutable
            {
                auto result = std::async([=]() { service->removeLiquidity(object{
                    object::pair{std::string("userVault"), Keypair->generate()}, 
                    object::pair{std::string("poolId"), std::string("dummy-pool-1")}, 
                    object::pair{std::string("lpTokenAmountLamports"), std::string("100000000")}, 
                    object::pair{std::string("slippageBps"), 50}
                }); });
                expect(result["success"])->toBe(true);
                expect(result["transactionId"])->toMatch((new RegExp(std::string("^dummy-tx-\d+"))));
                expect(result["tokensReceived"])->toBeDefined();
                expect(result["tokensReceived"]["length"])->toBe(2);
                expect(const_(result["tokensReceived"])[0]["symbol"])->toBe(std::string("SOL"));
                expect(const_(result["tokensReceived"])[1]["symbol"])->toBe(std::string("USDC"));
            }
            );
        }
        );
        describe(std::string("getLpPositionDetails"), [=]() mutable
        {
            it(std::string("should return mock LP position details"), [=]() mutable
            {
                auto userPublicKey = Keypair->generate()->publicKey->toBase58();
                auto positionId = std::string("dummy-lp-mint-dummy-pool-1");
                auto details = std::async([=]() { service->getLpPositionDetails(userPublicKey, positionId); });
                expect(details)->toBeDefined();
                expect(details["dex"])->toBe(std::string("dummy"));
                expect(details["poolId"])->toBe(std::string("dummy-pool-1"));
                expect(details["valueUsd"])->toBe(1000);
                expect(details["underlyingTokens"]["length"])->toBe(2);
                expect(details["lpTokenBalance"]["address"])->toBe(positionId);
            }
            );
        }
        );
        describe(std::string("getMarketDataForPools"), [=]() mutable
        {
            it(std::string("should return random market data for given pool IDs"), [=]() mutable
            {
                auto poolIds = array<string>{ std::string("dummy-pool-1"), std::string("dummy-stable-pool-2") };
                auto marketData = std::async([=]() { service->getMarketDataForPools(poolIds); });
                expect(Object->keys(marketData)->get_length())->toBe(2);
                expect((*const_(marketData))[std::string("dummy-pool-1")])->toBeDefined();
                expect((*const_(marketData))[std::string("dummy-pool-1")]->apy)->toBeTypeOf(std::string("number"));
                expect((*const_(marketData))[std::string("dummy-pool-1")]->tvl)->toBeTypeOf(std::string("number"));
                expect((*const_(marketData))[std::string("dummy-stable-pool-2")])->toBeDefined();
            }
            );
        }
        );
    }
    );
}

MAIN
