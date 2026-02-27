#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-dummy-services/src/tokenData/__tests__/service.test.h"

void Main(void)
{
    describe(std:("DummyTokenDataService"), [=]() mutable
    {
        shared<std::shared_ptr<DummyTokenDataService>> service;
        shared mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
        beforeEach([=]() mutable
        {
            service = std::make_shared<DummyTokenDataService>(mockRuntime);
            std::async([=]() { service->start(); });
        }
        );
        describe(std:("getTokenDetails"), [=]() mutable
        {
            it(std:("should return token details for a given address"), [=]() mutable
            {
                auto address = std:("So11111111111111111111111111111111111111112");
                auto tokenData = std::async([=]() { service->getTokenDetails(address, std:("solana")); });
                expect(tokenData)->toBeDefined();
                expect(tokenData["address"])->toBe(address);
                expect(tokenData["chain"])->toBe(std:("solana"));
                expect(tokenData["sourceProvider"])->toBe(std:("dummy"));
                expect(tokenData["id"])->toBe(std:("solana:") + address + string_empty);
            }
            );
            it(std:("should generate consistent symbol from address"), [=]() mutable
            {
                auto address = std:("So11111111111111111111111111111111111111112");
                auto tokenData = std::async([=]() { service->getTokenDetails(address, std:("solana")); });
                expect(tokenData["symbol"])->toBe(std:("1111"));
                expect(tokenData["name"])->toBe(std:("Dummy Token 1111"));
            }
            );
            it(std:("should include all required fields"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std:("test-address"), std:("ethereum")); });
                expect(tokenData)->toHaveProperty(std:("price"));
                expect(tokenData)->toHaveProperty(std:("priceChange24hPercent"));
                expect(tokenData)->toHaveProperty(std:("volume24hUSD"));
                expect(tokenData)->toHaveProperty(std:("marketCapUSD"));
                expect(tokenData)->toHaveProperty(std:("liquidity"));
                expect(tokenData)->toHaveProperty(std:("holders"));
                expect(tokenData)->toHaveProperty(std:("logoURI"));
                expect(tokenData)->toHaveProperty(std:("decimals"));
                expect(tokenData)->toHaveProperty(std:("lastUpdatedAt"));
                expect(tokenData)->toHaveProperty(std:("raw"));
            }
            );
            it(std:("should always return 18 decimals"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std:("any-address"), std:("any-chain")); });
                expect(tokenData["decimals"])->toBe(18);
            }
            );
        }
        );
        describe(std:("getTrendingTokens"), [=]() mutable
        {
            it(std:("should return requested number of trending tokens"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->getTrendingTokens(std:("solana"), 5); });
                expect(tokens)->toHaveLength(5);
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->chain)->toBe(std:("solana"));
                    expect(token->sourceProvider)->toBe(std:("dummy"));
                }
                );
            }
            );
            it(std:("should use default values when parameters are omitted"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->getTrendingTokens(); });
                expect(tokens)->toHaveLength(10);
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->chain)->toBe(std:("solana"));
                }
                );
            }
            );
            it(std:("should generate random but valid data for each token"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->getTrendingTokens(std:("ethereum"), 3); });
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->price)->toBeGreaterThanOrEqual(0);
                    expect(token->price)->toBeLessThanOrEqual(100);
                    expect(token->priceChange24hPercent)->toBeGreaterThanOrEqual(-10);
                    expect(token->priceChange24hPercent)->toBeLessThanOrEqual(10);
                    expect(token->volume24hUSD)->toBeGreaterThanOrEqual(0);
                    expect(token->marketCapUSD)->toBeGreaterThanOrEqual(0);
                    expect(token->liquidity)->toBeGreaterThanOrEqual(0);
                    expect(token->holders)->toBeGreaterThanOrEqual(0);
                    expect(token->holders)->toBeLessThanOrEqual(10000);
                }
                );
            }
            );
        }
        );
        describe(std:("searchTokens"), [=]() mutable
        {
            it(std:("should return tokens matching the query"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->searchTokens(std:("BTC"), std:("ethereum"), 3); });
                expect(tokens)->toHaveLength(3);
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->symbol)->toBe(std:("BTC"));
                    expect(token->name)->toBe(std:("Dummy Token BTC"));
                    expect(token->chain)->toBe(std:("ethereum"));
                }
                );
            }
            );
            it(std:("should use default values when optional parameters are omitted"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->searchTokens(std:("ETH")); });
                expect(tokens)->toHaveLength(5);
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->symbol)->toBe(std:("ETH"));
                    expect(token->chain)->toBe(std:("solana"));
                }
                );
            }
            );
            it(std:("should uppercase the query for symbol"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->searchTokens(std:("usdc")); });
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->symbol)->toBe(std:("USDC"));
                    expect(token->name)->toBe(std:("Dummy Token USDC"));
                }
                );
            }
            );
        }
        );
        describe(std:("getTokensByAddresses"), [=]() mutable
        {
            it(std:("should return tokens for all provided addresses"), [=]() mutable
            {
                shared addresses = array<string>{ std:("address1"), std:("address2"), std:("address3") };
                auto tokens = std::async([=]() { service->getTokensByAddresses(addresses, std:("polygon")); });
                expect(tokens)->toHaveLength(3);
                tokens->forEach([=](auto token, auto index) mutable
                {
                    expect(token->address)->toBe(const_(addresses)[index]);
                    expect(token->chain)->toBe(std:("polygon"));
                    expect(token->id)->toBe(std:("polygon:") + const_(addresses)[index] + string_empty);
                }
                );
            }
            );
            it(std:("should handle empty addresses array"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->getTokensByAddresses(array<any>(), std:("solana")); });
                expect(tokens)->toHaveLength(0);
            }
            );
            it(std:("should generate appropriate symbols from addresses"), [=]() mutable
            {
                auto addresses = array<string>{ std:("0xAbCdEf123456"), std:("0x9876543210Fe") };
                auto tokens = std::async([=]() { service->getTokensByAddresses(addresses, std:("ethereum")); });
                expect(const_(tokens)[0]->symbol)->toBe(std:("ABCD"));
                expect(const_(tokens)[1]->symbol)->toBe(std:("9876"));
            }
            );
        }
        );
        describe(std:("service lifecycle"), [=]() mutable
        {
            it(std:("should create service and support start/stop"), [=]() mutable
            {
                auto newService = std::make_shared<DummyTokenDataService>(mockRuntime);
                expect(newService)->toBeInstanceOf(DummyTokenDataService);
                std::async([=]() { expect(newService->start())->resolves->toBeUndefined(); });
                std::async([=]() { expect(newService->stop())->resolves->toBeUndefined(); });
            }
            );
            it(std:("should maintain service name"), [=]() mutable
            {
                auto newService = std::make_shared<DummyTokenDataService>(mockRuntime);
                expect(newService->serviceName)->toBe(std:("dummy-token-data"));
            }
            );
        }
        );
        describe(std:("data consistency"), [=]() mutable
        {
            it(std:("should always mark data as dummy"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std:("any-address"), std:("any-chain")); });
                expect(tokenData["raw"]["dummyData"])->toBe(true);
            }
            );
            it(std:("should use placeholder logo URI"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std:("any-address"), std:("any-chain")); });
                expect(tokenData["logoURI"])->toBe(std:("https://via.placeholder.com/150"));
            }
            );
            it(std:("should generate valid timestamps"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std:("any-address"), std:("any-chain")); });
                auto timestamp = tokenData["lastUpdatedAt"];
                expect(timestamp)->toBeInstanceOf(Date);
                expect(timestamp["getTime"]())->toBeLessThanOrEqual(Date->now());
                expect(timestamp["getTime"]())->toBeGreaterThan(Date->now() - 1000);
            }
            );
        }
        );
    }
    );
}

MAIN
