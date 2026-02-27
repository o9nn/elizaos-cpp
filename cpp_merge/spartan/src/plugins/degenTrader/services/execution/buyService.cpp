#include "buyService.hpp"

BuyService::BuyService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService, std::shared_ptr<TradeMemoryService> tradeMemoryService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
    this->validationService = std::make_shared<TokenValidationService>(runtime, walletService, dataService, analyticsService);
    this->calculationService = std::make_shared<TradeCalculationService>(runtime, walletService, dataService, analyticsService);
    this->tradeMemoryService = tradeMemoryService;
}

std::shared_ptr<Promise<void>> BuyService::initialize()
{
    logger->info(std:("Initializing buy service"));
    this->runtime->registerEvent(std:("SPARTAN_TRADE_BUY_SIGNAL"), std::bind(&BuyService::handleBuySignal, this, std::placeholders::_1)->bind(shared_from_this()));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BuyService::stop()
{
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BuyService::handleBuySignal(any params)
{
    auto TRADER_BUY_KUMA = this->runtime->getSetting(std:("TRADER_BUY_KUMA"));
    if (TRADER_BUY_KUMA) {
        fetch(TRADER_BUY_KUMA)->_catch([=](auto e) mutable
        {
            logger->error(std:("TRADER_BUY_KUMA err"), e);
        }
        );
    }
    auto signal = object{
        object::pair{std:("positionId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
        object::pair{std:("tokenAddress"), params["recommend_buy_address"]}, 
        object::pair{std:("entityId"), std:("default")}, 
        object::pair{std:("tradeAmount"), params["buy_amount"]}, 
        object::pair{std:("expectedOutAmount"), std:("0")}
    };
    std::async([=]() { this->updateExpectedOutAmount(signal); });
    this->executeBuy(signal)->then([=](auto result) mutable
    {
        logger->info(std:("executeBuy - result"), result);
    }
    );
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> BuyService::updateExpectedOutAmount(std::shared_ptr<BuySignalMessage> signal)
{
    if (!signal->tradeAmount) return std::shared_ptr<Promise<void>>();
    try
    {
        auto quoteResponse = std::async([=]() { fetch(std:("https://quote-api.jup.ag/v6/quote?inputMint=So11111111111111111111111111111111111111112&outputMint=") + signal->tokenAddress + std:("&amount=") + Math->round(Number(signal->tradeAmount) * 1000000000) + std:("&slippageBps=0")); });
        if (quoteResponse->ok) {
            auto quoteData = std::async([=]() { quoteResponse->json(); });
            signal->expectedOutAmount = quoteData["outAmount"];
        }
    }
    catch (const any& error)
    {
        logger->warn(std:("Failed to get expected out amount for buy"), object{
            object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}
        });
    }
}

std::shared_ptr<Promise<object>> BuyService::executeBuy(std::shared_ptr<BuySignalMessage> signal)
{
    try
    {
        if (!signal) {
            throw any(std::make_shared<Error>(std:("No signal data in buy task")));
        }
        auto validation = std::async([=]() { this->validationService->validateTokenForTrading(signal->tokenAddress); });
        if (!validation["isValid"]) {
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), validation["reason"]}
            };
        }
        auto marketData = std::async([=]() { this->dataService->getTokenMarketData(signal->tokenAddress); });
        auto walletBalance = std::async([=]() { this->walletService->getBalance(); });
        auto buyAmount = std::async([=]() { this->calculationService->calculateOptimalBuyAmount(object{
            object::pair{std:("tokenAddress"), signal->tokenAddress}, 
            object::pair{std:("walletBalance"), std:("walletBalance")}, 
            object::pair{std:("signal"), std:("signal")}
        }); });
        if (buyAmount <= 0) {
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Buy amount too small")}
            };
        }
        auto slippageBps = std::async([=]() { this->calculationService->calculateDynamicSlippage(signal->tokenAddress, buyAmount, false); });
        auto wallet = std::async([=]() { this->walletService->getWallet(); });
        auto result = std::async([=]() { wallet["buy"](object{
            object::pair{std:("tokenAddress"), signal->tokenAddress}, 
            object::pair{std:("amountInSol"), buyAmount}, 
            object::pair{std:("slippageBps"), std:("slippageBps")}
        }); });
        if (result->success) {
            std::async([=]() { this->tradeMemoryService->createTrade(object{
                object::pair{std:("tokenAddress"), signal->tokenAddress}, 
                object::pair{std:("chain"), std:("solana")}, 
                object::pair{std:("type"), std:("BUY")}, 
                object::pair{std:("amount"), buyAmount->toString()}, 
                object::pair{std:("price"), marketData["priceUsd"]->toString()}, 
                object::pair{std:("txHash"), result->signature}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("slippage"), slippageBps}, 
                    object::pair{std:("expectedAmount"), signal->expectedOutAmount}, 
                    object::pair{std:("receivedAmount"), result->outAmount}, 
                    object::pair{std:("valueUsd"), result->swapUsdValue}
                }}
            }); });
            if (result->outAmount) {
                std::async([=]() { this->analyticsService->trackSlippageImpact(signal->tokenAddress, OR((signal->expectedOutAmount), (std:("0"))), result->outAmount, slippageBps, false); });
            }
        }
        return result;
    }
    catch (const any& error)
    {
        logger->error(std:("Error executing buy task:"), error);
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}
        };
    }
}

BuyService::BuyService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
}

