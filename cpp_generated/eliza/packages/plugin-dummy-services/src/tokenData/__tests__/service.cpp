#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-dummy-services/src/tokenData/__tests__/service.test.h"

void Main(void)
{
    describe(std::string("DummyTokenDataService"), [=]() mutable
    {
        shared<std::shared_ptr<DummyTokenDataService>> service;
        shared mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
        beforeEach([=]() mutable
        {
            service = std::make_shared<DummyTokenDataService>(mockRuntime);
            std::async([=]() { service->start(); });
        }
        );
        describe(std::string("getTokenDetails"), [=]() mutable
        {
            it(std::string("should return token details for a given address"), [=]() mutable
            {
                auto address = std::string("So11111111111111111111111111111111111111112");
                auto tokenData = std::async([=]() { service->getTokenDetails(address, std::string("solana")); });
                expect(tokenData)->toBeDefined();
                expect(tokenData["address"])->toBe(address);
                expect(tokenData["chain"])->toBe(std::string("solana"));
                expect(tokenData["sourceProvider"])->toBe(std::string("dummy"));
                expect(tokenData["id"])->toBe(std::string("solana:") + address + string_empty);
            }
            );
            it(std::string("should generate consistent symbol from address"), [=]() mutable
            {
                auto address = std::string("So11111111111111111111111111111111111111112");
                auto tokenData = std::async([=]() { service->getTokenDetails(address, std::string("solana")); });
                expect(tokenData["symbol"])->toBe(std::string("1111"));
                expect(tokenData["name"])->toBe(std::string("Dummy Token 1111"));
            }
            );
            it(std::string("should include all required fields"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std::string("test-address"), std::string("ethereum")); });
                expect(tokenData)->toHaveProperty(std::string("price"));
                expect(tokenData)->toHaveProperty(std::string("priceChange24hPercent"));
                expect(tokenData)->toHaveProperty(std::string("volume24hUSD"));
                expect(tokenData)->toHaveProperty(std::string("marketCapUSD"));
                expect(tokenData)->toHaveProperty(std::string("liquidity"));
                expect(tokenData)->toHaveProperty(std::string("holders"));
                expect(tokenData)->toHaveProperty(std::string("logoURI"));
                expect(tokenData)->toHaveProperty(std::string("decimals"));
                expect(tokenData)->toHaveProperty(std::string("lastUpdatedAt"));
                expect(tokenData)->toHaveProperty(std::string("raw"));
            }
            );
            it(std::string("should always return 18 decimals"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std::string("any-address"), std::string("any-chain")); });
                expect(tokenData["decimals"])->toBe(18);
            }
            );
        }
        );
        describe(std::string("getTrendingTokens"), [=]() mutable
        {
            it(std::string("should return requested number of trending tokens"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->getTrendingTokens(std::string("solana"), 5); });
                expect(tokens)->toHaveLength(5);
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->chain)->toBe(std::string("solana"));
                    expect(token->sourceProvider)->toBe(std::string("dummy"));
                }
                );
            }
            );
            it(std::string("should use default values when parameters are omitted"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->getTrendingTokens(); });
                expect(tokens)->toHaveLength(10);
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->chain)->toBe(std::string("solana"));
                }
                );
            }
            );
            it(std::string("should generate random but valid data for each token"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->getTrendingTokens(std::string("ethereum"), 3); });
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
        describe(std::string("searchTokens"), [=]() mutable
        {
            it(std::string("should return tokens matching the query"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->searchTokens(std::string("BTC"), std::string("ethereum"), 3); });
                expect(tokens)->toHaveLength(3);
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->symbol)->toBe(std::string("BTC"));
                    expect(token->name)->toBe(std::string("Dummy Token BTC"));
                    expect(token->chain)->toBe(std::string("ethereum"));
                }
                );
            }
            );
            it(std::string("should use default values when optional parameters are omitted"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->searchTokens(std::string("ETH")); });
                expect(tokens)->toHaveLength(5);
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->symbol)->toBe(std::string("ETH"));
                    expect(token->chain)->toBe(std::string("solana"));
                }
                );
            }
            );
            it(std::string("should uppercase the query for symbol"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->searchTokens(std::string("usdc")); });
                tokens->forEach([=](auto token) mutable
                {
                    expect(token->symbol)->toBe(std::string("USDC"));
                    expect(token->name)->toBe(std::string("Dummy Token USDC"));
                }
                );
            }
            );
        }
        );
        describe(std::string("getTokensByAddresses"), [=]() mutable
        {
            it(std::string("should return tokens for all provided addresses"), [=]() mutable
            {
                shared addresses = array<string>{ std::string("address1"), std::string("address2"), std::string("address3") };
                auto tokens = std::async([=]() { service->getTokensByAddresses(addresses, std::string("polygon")); });
                expect(tokens)->toHaveLength(3);
                tokens->forEach([=](auto token, auto index) mutable
                {
                    expect(token->address)->toBe(const_(addresses)[index]);
                    expect(token->chain)->toBe(std::string("polygon"));
                    expect(token->id)->toBe(std::string("polygon:") + const_(addresses)[index] + string_empty);
                }
                );
            }
            );
            it(std::string("should handle empty addresses array"), [=]() mutable
            {
                auto tokens = std::async([=]() { service->getTokensByAddresses(array<any>(), std::string("solana")); });
                expect(tokens)->toHaveLength(0);
            }
            );
            it(std::string("should generate appropriate symbols from addresses"), [=]() mutable
            {
                auto addresses = array<string>{ std::string("0xAbCdEf123456"), std::string("0x9876543210Fe") };
                auto tokens = std::async([=]() { service->getTokensByAddresses(addresses, std::string("ethereum")); });
                expect(const_(tokens)[0]->symbol)->toBe(std::string("ABCD"));
                expect(const_(tokens)[1]->symbol)->toBe(std::string("9876"));
            }
            );
        }
        );
        describe(std::string("service lifecycle"), [=]() mutable
        {
            it(std::string("should create service and support start/stop"), [=]() mutable
            {
                auto newService = std::make_shared<DummyTokenDataService>(mockRuntime);
                expect(newService)->toBeInstanceOf(DummyTokenDataService);
                std::async([=]() { expect(newService->start())->resolves->toBeUndefined(); });
                std::async([=]() { expect(newService->stop())->resolves->toBeUndefined(); });
            }
            );
            it(std::string("should maintain service name"), [=]() mutable
            {
                auto newService = std::make_shared<DummyTokenDataService>(mockRuntime);
                expect(newService->serviceName)->toBe(std::string("dummy-token-data"));
            }
            );
        }
        );
        describe(std::string("data consistency"), [=]() mutable
        {
            it(std::string("should always mark data as dummy"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std::string("any-address"), std::string("any-chain")); });
                expect(tokenData["raw"]["dummyData"])->toBe(true);
            }
            );
            it(std::string("should use placeholder logo URI"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std::string("any-address"), std::string("any-chain")); });
                expect(tokenData["logoURI"])->toBe(std::string("https://via.placeholder.com/150"));
            }
            );
            it(std::string("should generate valid timestamps"), [=]() mutable
            {
                auto tokenData = std::async([=]() { service->getTokenDetails(std::string("any-address"), std::string("any-chain")); });
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
