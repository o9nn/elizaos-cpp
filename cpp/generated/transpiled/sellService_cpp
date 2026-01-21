#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/execution/sellService.h"

SellService::SellService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService, std::shared_ptr<TradeMemoryService> tradeMemoryService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
    this->validationService = std::make_shared<TokenValidationService>(runtime, walletService, dataService, analyticsService);
    this->calculationService = std::make_shared<TradeCalculationService>(runtime, walletService, dataService, analyticsService);
    this->tradeMemoryService = tradeMemoryService;
}

std::shared_ptr<Promise<void>> SellService::initialize()
{
    logger->info(std::string("Initializing sell service"));
    this->runtime->registerEvent(std::string("SPARTAN_TRADE_SELL_SIGNAL"), std::bind(&SellService::handleSellSignal, this, std::placeholders::_1)->bind(shared_from_this()));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> SellService::stop()
{
    this->pendingSells = object{};
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> SellService::handleSellSignal(any params)
{
    auto TRADER_SELL_KUMA = this->runtime->getSetting(std::string("TRADER_SELL_KUMA"));
    if (TRADER_SELL_KUMA) {
        fetch(TRADER_SELL_KUMA)->_catch([=](auto e) mutable
        {
            logger->error(std::string("TRADER_SELL_KUMA err"), e);
        }
        );
    }
    auto signal = object{
        object::pair{std::string("positionId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std::string("tokenAddress"), params["recommend_sell_address"]}, 
        object::pair{std::string("amount"), params["sell_amount"]}, 
        object::pair{std::string("entityId"), std::string("default")}, 
        object::pair{std::string("slippage"), OR((params["slippage"]), (100))}
    };
    std::async([=]() { this->updateExpectedOutAmount(signal); });
    this->executeSell(signal)->then([=](auto result) mutable
    {
        logger->info(std::string("executeSell - result"), result);
    }
    );
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> SellService::updateExpectedOutAmount(any signal)
{
    if (!signal->amount) return std::shared_ptr<Promise<void>>();
    try
    {
        auto quoteResponse = std::async([=]() { fetch(std::string("https://quote-api.jup.ag/v6/quote?inputMint=") + signal->tokenAddress + std::string("&outputMint=So11111111111111111111111111111111111111112&amount=") + Math->round(Number(signal->amount) * 1000000000) + std::string("&slippageBps=") + (OR((signal->slippage), (100))) + string_empty); });
        if (quoteResponse->ok) {
            auto quoteData = std::async([=]() { quoteResponse->json(); });
            signal->expectedOutAmount = quoteData["outAmount"];
        }
    }
    catch (const any& error)
    {
        logger->warn(std::string("Failed to get expected out amount for sell"), object{
            object::pair{std::string("error"), (is<Error>(error)) ? error->message : String(error)}
        });
    }
}

std::shared_ptr<Promise<object>> SellService::executeSell(any signal)
{
    try
    {
        if (!signal) {
            throw any(std::make_shared<Error>(std::string("No signal data in sell task")));
        }
        auto tokenBalance = std::async([=]() { getTokenBalance(this->runtime, signal->tokenAddress); });
        if (!tokenBalance) {
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("No token balance found")}
            };
        }
        auto sellAmount = toBN(signal->amount)->times(Math.pow(10, tokenBalance->decimals));
        if (sellAmount->gt(toBN(tokenBalance->balance))) {
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Insufficient token balance. Requested: ") + sellAmount->toString() + std::string(", Available: ") + tokenBalance->balance + string_empty}
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
                    object::pair{std::string("tokenAddress"), signal->tokenAddress}, 
                    object::pair{std::string("amount"), sellAmount->toString()}, 
                    object::pair{std::string("slippage"), slippageBps}, 
                    object::pair{std::string("dex"), std::string("jup")}, 
                    object::pair{std::string("action"), std::string("SELL")}
                }); });
                auto marketData = std::async([=]() { this->dataService->getTokenMarketData(signal->tokenAddress); });
                if (result["success"]) {
                    std::async([=]() { this->tradeMemoryService->createTrade(object{
                        object::pair{std::string("tokenAddress"), signal->tokenAddress}, 
                        object::pair{std::string("chain"), std::string("solana")}, 
                        object::pair{std::string("type"), std::string("SELL")}, 
                        object::pair{std::string("amount"), sellAmount->toString()}, 
                        object::pair{std::string("price"), marketData["priceUsd"]->toString()}, 
                        object::pair{std::string("txHash"), result["signature"]}, 
                        object::pair{std::string("metadata"), object{
                            object::pair{std::string("slippage"), slippageBps}, 
                            object::pair{std::string("expectedAmount"), OR((signal->expectedOutAmount), (std::string("0")))}, 
                            object::pair{std::string("receivedAmount"), OR((result["receivedAmount"]), (std::string("0")))}, 
                            object::pair{std::string("valueUsd"), OR((result["receivedValue"]), (std::string("0")))}
                        }}
                    }); });
                    std::async([=]() { this->analyticsService->trackSlippageImpact(signal->tokenAddress, OR((signal->expectedOutAmount), (std::string("0"))), OR((result["receivedAmount"]), (std::string("0"))), slippageBps, true); });
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
        logger->error(std::string("Error executing sell task:"), error);
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), (is<Error>(error)) ? error->message : String(error)}
        };
    }
}

SellService::SellService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
}

