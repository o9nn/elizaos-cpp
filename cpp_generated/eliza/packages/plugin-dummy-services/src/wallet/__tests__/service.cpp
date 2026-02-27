#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-dummy-services/src/wallet/__tests__/service.test.h"

void Main(void)
{
    describe(std::string("DummyWalletService"), [=]() mutable
    {
        shared<std::shared_ptr<DummyWalletService>> service;
        shared mockRuntime = as<std::shared_ptr<AgentRuntime>>(object{});
        beforeEach([=]() mutable
        {
            service = std::make_shared<DummyWalletService>(mockRuntime);
            std::async([=]() { service->start(); });
        }
        );
        describe(std::string("initialization"), [=]() mutable
        {
            it(std::string("should initialize with default USDC balance"), [=]() mutable
            {
                auto balance = std::async([=]() { service->getBalance(std::string("USDC")); });
                expect(balance)->toBe(10000);
            }
            );
            it(std::string("should have empty balances for other assets"), [=]() mutable
            {
                auto solBalance = std::async([=]() { service->getBalance(std::string("SOL")); });
                expect(solBalance)->toBe(0);
            }
            );
        }
        );
        describe(std::string("addFunds"), [=]() mutable
        {
            it(std::string("should add funds to an existing balance"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std::string("USDC"), 500); });
                auto balance = std::async([=]() { service->getBalance(std::string("USDC")); });
                expect(balance)->toBe(10500);
            }
            );
            it(std::string("should create a new balance for a new asset"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std::string("SOL"), 10); });
                auto balance = std::async([=]() { service->getBalance(std::string("SOL")); });
                expect(balance)->toBe(10);
            }
            );
            it(std::string("should handle multiple additions"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std::string("SOL"), 5); });
                std::async([=]() { service->addFunds(std::string("SOL"), 3); });
                auto balance = std::async([=]() { service->getBalance(std::string("SOL")); });
                expect(balance)->toBe(8);
            }
            );
        }
        );
        describe(std::string("setPortfolioHolding"), [=]() mutable
        {
            it(std::string("should set portfolio holding for non-quote asset"), [=]() mutable
            {
                std::async([=]() { service->setPortfolioHolding(std::string("SOL"), 5, 100); });
                auto balance = std::async([=]() { service->getBalance(std::string("SOL")); });
                expect(balance)->toBe(5);
            }
            );
            it(std::string("should convert to addFunds for quote asset"), [=]() mutable
            {
                std::async([=]() { service->setPortfolioHolding(std::string("USDC"), 100, 1); });
                auto balance = std::async([=]() { service->getBalance(std::string("USDC")); });
                expect(balance)->toBe(10100);
            }
            );
        }
        );
        describe(std::string("resetWallet"), [=]() mutable
        {
            it(std::string("should reset wallet with new initial cash"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std::string("SOL"), 10); });
                std::async([=]() { service->resetWallet(5000); });
                auto usdcBalance = std::async([=]() { service->getBalance(std::string("USDC")); });
                auto solBalance = std::async([=]() { service->getBalance(std::string("SOL")); });
                expect(usdcBalance)->toBe(5000);
                expect(solBalance)->toBe(0);
            }
            );
            it(std::string("should support different quote assets"), [=]() mutable
            {
                std::async([=]() { service->resetWallet(2000, std::string("USDT")); });
                auto usdtBalance = std::async([=]() { service->getBalance(std::string("USDT")); });
                auto usdcBalance = std::async([=]() { service->getBalance(std::string("USDC")); });
                expect(usdtBalance)->toBe(2000);
                expect(usdcBalance)->toBe(0);
            }
            );
        }
        );
        describe(std::string("getPortfolio"), [=]() mutable
        {
            it(std::string("should return correct portfolio structure"), [=]() mutable
            {
                auto portfolio = std::async([=]() { service->getPortfolio(); });
                expect(portfolio)->toHaveProperty(std::string("totalValueUsd"));
                expect(portfolio)->toHaveProperty(std::string("assets"));
                expect(Array->isArray(portfolio->assets))->toBe(true);
                expect(portfolio->totalValueUsd)->toBe(10000);
            }
            );
            it(std::string("should calculate correct total value with multiple assets"), [=]() mutable
            {
                std::async([=]() { service->setPortfolioHolding(std::string("SOL"), 2, 100); });
                std::async([=]() { service->setPortfolioHolding(std::string("ETH"), 1, 2000); });
                auto portfolio = std::async([=]() { service->getPortfolio(); });
                expect(portfolio->totalValueUsd)->toBe(12200);
                expect(portfolio->assets->length)->toBe(3);
            }
            );
            it(std::string("should include all required fields in assets"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std::string("SOL"), 5); });
                auto portfolio = std::async([=]() { service->getPortfolio(); });
                auto solAsset = portfolio->assets->find([=](auto a) mutable
                {
                    return a["symbol"] == std::string("SOL");
                }
                );
                expect(solAsset)->toBeDefined();
                expect(solAsset)->toHaveProperty(std::string("address"));
                expect(solAsset)->toHaveProperty(std::string("symbol"));
                expect(solAsset)->toHaveProperty(std::string("balance"));
                expect(solAsset)->toHaveProperty(std::string("decimals"));
                expect(solAsset)->toHaveProperty(std::string("quantity"));
                expect(solAsset)->toHaveProperty(std::string("averagePrice"));
                expect(solAsset)->toHaveProperty(std::string("value"));
            }
            );
        }
        );
        describe(std::string("transferSol"), [=]() mutable
        {
            it(std::string("should transfer SOL when sufficient balance exists"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std::string("SOL"), 5); });
                auto txHash = std::async([=]() { service->transferSol(std::string("from-address"), std::string("to-address"), 2000000000); });
                expect(txHash)->toMatch((new RegExp(std::string("^dummy-tx"))));
                auto balance = std::async([=]() { service->getBalance(std::string("SOL")); });
                expect(balance)->toBe(3);
            }
            );
            it(std::string("should throw error when insufficient balance"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std::string("SOL"), 1); });
                std::async([=]() { expect(service->transferSol(std::string("from-address"), std::string("to-address"), 2000000000))->rejects->toThrow(std::string("Insufficient SOL balance")); });
            }
            );
            it(std::string("should handle transfer when no SOL balance exists"), [=]() mutable
            {
                std::async([=]() { expect(service->transferSol(std::string("from-address"), std::string("to-address"), 1000000000))->rejects->toThrow(std::string("Insufficient SOL balance")); });
            }
            );
        }
        );
        describe(std::string("static methods"), [=]() mutable
        {
            it(std::string("should create instance through static start method"), [=]() mutable
            {
                auto instance = std::async([=]() { DummyWalletService::start(mockRuntime); });
                expect(instance)->toBeInstanceOf(DummyWalletService);
                auto balance = std::async([=]() { instance->getBalance(std::string("USDC")); });
                expect(balance)->toBe(10000);
            }
            );
        }
        );
        describe(std::string("stop"), [=]() mutable
        {
            it(std::string("should clear all balances when stopped"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std::string("SOL"), 10); });
                std::async([=]() { service->stop(); });
                auto portfolio = std::async([=]() { service->getPortfolio(); });
                expect(portfolio->assets->length)->toBe(0);
                expect(portfolio->totalValueUsd)->toBe(0);
            }
            );
        }
        );
    }
    );
}

MAIN
