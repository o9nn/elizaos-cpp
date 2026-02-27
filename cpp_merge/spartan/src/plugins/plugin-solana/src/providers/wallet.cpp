#include "wallet.hpp"

std::shared_ptr<Provider> walletProvider = object{
    object::pair{std:("name"), std:("solana-wallet")}, 
    object::pair{std:("description"), std:("your solana wallet information")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto _message, auto state = undefined) mutable
    {
        try
        {
            auto portfolioCache = std::async([=]() { runtime->getCache<std::shared_ptr<WalletPortfolio>>(SOLANA_WALLET_DATA_CACHE_KEY); });
            if (!portfolioCache) {
                logger->info(std:("solana::wallet provider - portfolioCache is not ready"));
                return object{
                    object::pair{std:("data"), nullptr}, 
                    object::pair{std:("values"), object{}}, 
                    object::pair{std:("text"), string_empty}
                };
            }
            auto solanaService = runtime->getService(std:("solana"));
            auto pubkeyStr = string_empty;
            if (solanaService) {
                pubkeyStr = std:(" (") + solanaService->publicKey->toBase58() + std:(")");
            }
            auto portfolio = portfolioCache;
            auto agentName = OR((OR((state->agentName), (runtime->character->name))), (std:("The agent")));
            shared values = object{
                object::pair{std:("total_usd"), ((std::make_shared<BigNumber>(portfolio->totalUsd)))->toFixed(2)}, 
                object::pair{std:("total_sol"), portfolio->totalSol->toString()}
            };
            portfolio->items->forEach([=](auto item, auto index) mutable
            {
                if (((std::make_shared<BigNumber>(item["uiAmount"])))->isGreaterThan(0)) {
                    values[std:("token_") + index + std:("_name")] = item["name"];
                    values[std:("token_") + index + std:("_symbol")] = item["symbol"];
                    values[std:("token_") + index + std:("_amount")] = ((std::make_shared<BigNumber>(item["uiAmount"])))->toFixed(6);
                    values[std:("token_") + index + std:("_usd")] = ((std::make_shared<BigNumber>(item["valueUsd"])))->toFixed(2);
                    values[std:("token_") + index + std:("_sol")] = item["valueSol"]["toString"]();
                }
            }
            );
            if (portfolio->prices) {
                values->sol_price = ((std::make_shared<BigNumber>(portfolio->prices->solana->usd)))->toFixed(2);
                values->btc_price = ((std::make_shared<BigNumber>(portfolio->prices->bitcoin->usd)))->toFixed(2);
                values->eth_price = ((std::make_shared<BigNumber>(portfolio->prices->ethereum->usd)))->toFixed(2);
            }
            auto text = std:("\
\
") + agentName + std:("'s Main Solana Wallet") + pubkeyStr + std:("\
");
            text += std:("Total Value: $") + values->total_usd + std:(" (") + values->total_sol + std:(" SOL)\
\
");
            text += std:("Token Balances:\
");
            auto nonZeroItems = portfolio->items->filter([=](auto item) mutable
            {
                return ((std::make_shared<BigNumber>(item["uiAmount"])))->isGreaterThan(0);
            }
            );
            if (nonZeroItems->length == 0) {
                text += std:("No tokens found with non-zero balance\
");
            } else {
                for (auto& item : nonZeroItems)
                {
                    auto valueUsd = ((std::make_shared<BigNumber>(item->valueUsd)))->toFixed(2);
                    text += string_empty + item->name + std:(" (") + item->symbol + std:("): ") + ((std::make_shared<BigNumber>(item->uiAmount)))->toFixed(6) + std:(" ($") + valueUsd + std:(" | ") + item->valueSol + std:(" SOL)\
");
                }
            }
            if (portfolio->prices) {
                text += std:("\
Market Prices:\
");
                text += std:("SOL: $") + values->sol_price + std:("\
");
                text += std:("BTC: $") + values->btc_price + std:("\
");
                text += std:("ETH: $") + values->eth_price + std:("\
");
            }
            return object{
                object::pair{std:("data"), portfolio}, 
                object::pair{std:("values"), values}, 
                object::pair{std:("text"), text}
            };
        }
        catch (const any& error)
        {
            console->error(std:("Error in Solana wallet provider:"), error);
            return object{
                object::pair{std:("data"), nullptr}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
