#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/execution/sellService.h"

SellService::SellService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService, std::shared_ptr<TradeMemoryService> tradeMemoryService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
    this->validationService = std::make_shared<TokenValidationService>(runtime, walletService, dataService, analyticsService);
    this->calculationService = std::make_shared<TradeCalculationService>(runtime, walletService, dataService, analyticsService);
    this->tradeMemoryService = tradeMemoryService;
}

std::shared_ptr<Promise<void>> SellService::initialize()
{
    logger->info(std:("Initializing sell service"));
    this->runtime->registerEvent(std:("SPARTAN_TRADE_SELL_SIGNAL"), std::bind(&SellService::handleSellSignal, this, std::placeholders::_1)->bind(shared_from_this()));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> SellService::stop()
{
    this->pendingSells = object{};
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> SellService::handleSellSignal(any params)
{
    auto TRADER_SELL_KUMA = this->runtime->getSetting(std:("TRADER_SELL_KUMA"));
    if (TRADER_SELL_KUMA) {
        fetch(TRADER_SELL_KUMA)->_catch([=](auto e) mutable
        {
            logger->error(std:("TRADER_SELL_KUMA err"), e);
        }
        );
    }
    auto signal = object{
        object::pair{std:("positionId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std:("tokenAddress"), params["recommend_sell_address"]}, 
        object::pair{std:("amount"), params["sell_amount"]}, 
        object::pair{std:("entityId"), std:("default")}, 
        object::pair{std:("slippage"), OR((params["slippage"]), (100))}
    };
    std::async([=]() { this->updateExpectedOutAmount(signal); });
    this->executeSell(signal)->then([=](auto result) mutable
    {
        logger->info(std:("executeSell - result"), result);
    }
    );
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> SellService::updateExpectedOutAmount(any signal)
{
    if (!signal->amount) return std::shared_ptr<Promise<void>>();
    try
    {
        auto quoteResponse = std::async([=]() { fetch(std:("https://quote-api.jup.ag/v6/quote?inputMint=") + signal->tokenAddress + std:("&outputMint=So11111111111111111111111111111111111111112&amount=") + Math->round(Number(signal->amount) * 1000000000) + std:("&slippageBps=") + (OR((signal->slippage), (100))) + string_empty); });
        if (quoteResponse->ok) {
            auto quoteData = std::async([=]() { quoteResponse->json(); });
            signal->expectedOutAmount = quoteData["outAmount"];
        }
    }
    catch (const any& error)
    {
        logger->warn(std:("Failed to get expected out amount for sell"), object{
            object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}
        });
    }
}

std::shared_ptr<Promise<object>> SellService::executeSell(any signal)
{
    try
    {
        if (!signal) {
            throw any(std::make_shared<Error>(std:("No signal data in sell task")));
        }
        auto tokenBalance = std::async([=]() { getTokenBalance(this->runtime, signal->tokenAddress); });
        if (!tokenBalance) {
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("No token balance found")}
            };
        }
        auto sellAmount = toBN(signal->amount)->times(Math.pow(10, tokenBalance->decimals));
        if (sellAmount->gt(toBN(tokenBalance->balance))) {
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Insufficient token balance. Requested: ") + sellAmount->toString() + std:(", Available: ") + tokenBalance->balance + string_empty}
            };
        }
        {
            utils::finally __finally6564_6848([&]() mutable
            {
                this->pendingSells[signal->tokenAddress] = (OR((const_(this->pendingSells)[signal->tokenAddress]), (toBN(0))))["minus"](sellAmount);
                if (const_(this->pendingSells)[signal->tokenAddress]->lte(toBN(0))) {
                    this->pendingSells.Delete(signal->tokenAddress);
                }
            });
            try
            {
                this->pendingSells[signal->tokenAddress] = (OR((const_(this->pendingSells)[signal->tokenAddress]), (toBN(0))))["plus"](sellAmount);
                auto slippageBps = std::async([=]() { this->calculationService->calculateDynamicSlippage(signal->tokenAddress, Number(sellAmount), true); });
                auto result = std::async([=]() { executeTrade(this->runtime, object{
                    object::pair{std:("tokenAddress"), signal->tokenAddress}, 
                    object::pair{std:("amount"), sellAmount->toString()}, 
                    object::pair{std:("slippage"), slippageBps}, 
                    object::pair{std:("dex"), std:("jup")}, 
                    object::pair{std:("action"), std:("SELL")}
                }); });
                auto marketData = std::async([=]() { this->dataService->getTokenMarketData(signal->tokenAddress); });
                if (result["success"]) {
                    std::async([=]() { this->tradeMemoryService->createTrade(object{
                        object::pair{std:("tokenAddress"), signal->tokenAddress}, 
                        object::pair{std:("chain"), std:("solana")}, 
                        object::pair{std:("type"), std:("SELL")}, 
                        object::pair{std:("amount"), sellAmount->toString()}, 
                        object::pair{std:("price"), marketData["priceUsd"]->toString()}, 
                        object::pair{std:("txHash"), result["signature"]}, 
                        object::pair{std:("metadata"), object{
                            object::pair{std:("slippage"), slippageBps}, 
                            object::pair{std:("expectedAmount"), OR((signal->expectedOutAmount), (std:("0")))}, 
                            object::pair{std:("receivedAmount"), OR((result["receivedAmount"]), (std:("0")))}, 
                            object::pair{std:("valueUsd"), OR((result["receivedValue"]), (std:("0")))}
                        }}
                    }); });
                    std::async([=]() { this->analyticsService->trackSlippageImpact(signal->tokenAddress, OR((signal->expectedOutAmount), (std:("0"))), OR((result["receivedAmount"]), (std:("0"))), slippageBps, true); });
                }
                return result;
            }
            catch (...)
            {
                throw;
            }
        }
    }
    catch (const any& error)
    {
        logger->error(std:("Error executing sell task:"), error);
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}
        };
    }
}

SellService::SellService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
}

