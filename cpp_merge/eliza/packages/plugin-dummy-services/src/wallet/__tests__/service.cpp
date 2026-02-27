#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-dummy-services/src/wallet/__tests__/service.test.h"

void Main(void)
{
    describe(std:("DummyWalletService"), [=]() mutable
    {
        shared<std::shared_ptr<DummyWalletService>> service;
        shared mockRuntime = as<std::shared_ptr<AgentRuntime>>(object{});
        beforeEach([=]() mutable
        {
            service = std::make_shared<DummyWalletService>(mockRuntime);
            std::async([=]() { service->start(); });
        }
        );
        describe(std:("initialization"), [=]() mutable
        {
            it(std:("should initialize with default USDC balance"), [=]() mutable
            {
                auto balance = std::async([=]() { service->getBalance(std:("USDC")); });
                expect(balance)->toBe(10000);
            }
            );
            it(std:("should have empty balances for other assets"), [=]() mutable
            {
                auto solBalance = std::async([=]() { service->getBalance(std:("SOL")); });
                expect(solBalance)->toBe(0);
            }
            );
        }
        );
        describe(std:("addFunds"), [=]() mutable
        {
            it(std:("should add funds to an existing balance"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std:("USDC"), 500); });
                auto balance = std::async([=]() { service->getBalance(std:("USDC")); });
                expect(balance)->toBe(10500);
            }
            );
            it(std:("should create a new balance for a new asset"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std:("SOL"), 10); });
                auto balance = std::async([=]() { service->getBalance(std:("SOL")); });
                expect(balance)->toBe(10);
            }
            );
            it(std:("should handle multiple additions"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std:("SOL"), 5); });
                std::async([=]() { service->addFunds(std:("SOL"), 3); });
                auto balance = std::async([=]() { service->getBalance(std:("SOL")); });
                expect(balance)->toBe(8);
            }
            );
        }
        );
        describe(std:("setPortfolioHolding"), [=]() mutable
        {
            it(std:("should set portfolio holding for non-quote asset"), [=]() mutable
            {
                std::async([=]() { service->setPortfolioHolding(std:("SOL"), 5, 100); });
                auto balance = std::async([=]() { service->getBalance(std:("SOL")); });
                expect(balance)->toBe(5);
            }
            );
            it(std:("should convert to addFunds for quote asset"), [=]() mutable
            {
                std::async([=]() { service->setPortfolioHolding(std:("USDC"), 100, 1); });
                auto balance = std::async([=]() { service->getBalance(std:("USDC")); });
                expect(balance)->toBe(10100);
            }
            );
        }
        );
        describe(std:("resetWallet"), [=]() mutable
        {
            it(std:("should reset wallet with new initial cash"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std:("SOL"), 10); });
                std::async([=]() { service->resetWallet(5000); });
                auto usdcBalance = std::async([=]() { service->getBalance(std:("USDC")); });
                auto solBalance = std::async([=]() { service->getBalance(std:("SOL")); });
                expect(usdcBalance)->toBe(5000);
                expect(solBalance)->toBe(0);
            }
            );
            it(std:("should support different quote assets"), [=]() mutable
            {
                std::async([=]() { service->resetWallet(2000, std:("USDT")); });
                auto usdtBalance = std::async([=]() { service->getBalance(std:("USDT")); });
                auto usdcBalance = std::async([=]() { service->getBalance(std:("USDC")); });
                expect(usdtBalance)->toBe(2000);
                expect(usdcBalance)->toBe(0);
            }
            );
        }
        );
        describe(std:("getPortfolio"), [=]() mutable
        {
            it(std:("should return correct portfolio structure"), [=]() mutable
            {
                auto portfolio = std::async([=]() { service->getPortfolio(); });
                expect(portfolio)->toHaveProperty(std:("totalValueUsd"));
                expect(portfolio)->toHaveProperty(std:("assets"));
                expect(Array->isArray(portfolio->assets))->toBe(true);
                expect(portfolio->totalValueUsd)->toBe(10000);
            }
            );
            it(std:("should calculate correct total value with multiple assets"), [=]() mutable
            {
                std::async([=]() { service->setPortfolioHolding(std:("SOL"), 2, 100); });
                std::async([=]() { service->setPortfolioHolding(std:("ETH"), 1, 2000); });
                auto portfolio = std::async([=]() { service->getPortfolio(); });
                expect(portfolio->totalValueUsd)->toBe(12200);
                expect(portfolio->assets->length)->toBe(3);
            }
            );
            it(std:("should include all required fields in assets"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std:("SOL"), 5); });
                auto portfolio = std::async([=]() { service->getPortfolio(); });
                auto solAsset = portfolio->assets->find([=](auto a) mutable
                {
                    return a["symbol"] == std:("SOL");
                }
                );
                expect(solAsset)->toBeDefined();
                expect(solAsset)->toHaveProperty(std:("address"));
                expect(solAsset)->toHaveProperty(std:("symbol"));
                expect(solAsset)->toHaveProperty(std:("balance"));
                expect(solAsset)->toHaveProperty(std:("decimals"));
                expect(solAsset)->toHaveProperty(std:("quantity"));
                expect(solAsset)->toHaveProperty(std:("averagePrice"));
                expect(solAsset)->toHaveProperty(std:("value"));
            }
            );
        }
        );
        describe(std:("transferSol"), [=]() mutable
        {
            it(std:("should transfer SOL when sufficient balance exists"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std:("SOL"), 5); });
                auto txHash = std::async([=]() { service->transferSol(std:("from-address"), std:("to-address"), 2000000000); });
                expect(txHash)->toMatch((new RegExp(std:("^dummy-tx"))));
                auto balance = std::async([=]() { service->getBalance(std:("SOL")); });
                expect(balance)->toBe(3);
            }
            );
            it(std:("should throw error when insufficient balance"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std:("SOL"), 1); });
                std::async([=]() { expect(service->transferSol(std:("from-address"), std:("to-address"), 2000000000))->rejects->toThrow(std:("Insufficient SOL balance")); });
            }
            );
            it(std:("should handle transfer when no SOL balance exists"), [=]() mutable
            {
                std::async([=]() { expect(service->transferSol(std:("from-address"), std:("to-address"), 1000000000))->rejects->toThrow(std:("Insufficient SOL balance")); });
            }
            );
        }
        );
        describe(std:("static methods"), [=]() mutable
        {
            it(std:("should create instance through static start method"), [=]() mutable
            {
                auto instance = std::async([=]() { DummyWalletService::start(mockRuntime); });
                expect(instance)->toBeInstanceOf(DummyWalletService);
                auto balance = std::async([=]() { instance->getBalance(std:("USDC")); });
                expect(balance)->toBe(10000);
            }
            );
        }
        );
        describe(std:("stop"), [=]() mutable
        {
            it(std:("should clear all balances when stopped"), [=]() mutable
            {
                std::async([=]() { service->addFunds(std:("SOL"), 10); });
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
