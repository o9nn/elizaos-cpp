#include "wallet.hpp"
#include <string>

std::shared_ptr<Provider> walletProvider = object{
    object::pair{std::string("name"), std::string("solana-wallet")}, 
    object::pair{std::string("description"), std::string("your solana wallet information")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message, auto state = std::nullopt) mutable
    {
        try
        {
            auto portfolioCache = std::async([=]() { runtime->getCache<std::shared_ptr<WalletPortfolio>>(SOLANA_WALLET_DATA_CACHE_KEY); });
            if (!portfolioCache) {
                logger->info(std::string("solana::wallet provider - portfolioCache is not ready"));
                return object{
                    object::pair{std::string("data"), nullptr}, 
                    object::pair{std::string("values"), object{}}, 
                    object::pair{std::string("text"), string_empty}
                };
            }
            auto solanaService = runtime->getService(std::string("solana"));
            auto pubkeyStr = string_empty;
            if (solanaService) {
                pubkeyStr = std::string(" (") + solanaService->publicKey->toBase58() + std::string(")");
            }
            auto portfolio = portfolioCache;
            auto agentName = OR((OR((state->agentName), (runtime->character->name))), (std::string("The agent")));
            shared values = object{
                object::pair{std::string("total_usd"), ((std::make_shared<BigNumber>(portfolio->totalUsd)))->toFixed(2)}, 
                object::pair{std::string("total_sol"), portfolio->totalSol->toString()}
            };
            portfolio->items->forEach([=](auto item, auto index) mutable
            {
                if (((std::make_shared<BigNumber>(item["uiAmount"])))->isGreaterThan(0)) {
                    values[std::string("token_") + index + std::string("_name")] = item["name"];
                    values[std::string("token_") + index + std::string("_symbol")] = item["symbol"];
                    values[std::string("token_") + index + std::string("_amount")] = ((std::make_shared<BigNumber>(item["uiAmount"])))->toFixed(6);
                    values[std::string("token_") + index + std::string("_usd")] = ((std::make_shared<BigNumber>(item["valueUsd"])))->toFixed(2);
                    values[std::string("token_") + index + std::string("_sol")] = item["valueSol"]["toString"]();
                }
            }
            );
            if (portfolio->prices) {
                values->sol_price = ((std::make_shared<BigNumber>(portfolio->prices->solana->usd)))->toFixed(2);
                values->btc_price = ((std::make_shared<BigNumber>(portfolio->prices->bitcoin->usd)))->toFixed(2);
                values->eth_price = ((std::make_shared<BigNumber>(portfolio->prices->ethereum->usd)))->toFixed(2);
            }
            auto text = std::string("\
\
") + agentName + std::string("'s Main Solana Wallet") + pubkeyStr + std::string("\
");
            text += std::string("Total Value: $") + values->total_usd + std::string(" (") + values->total_sol + std::string(" SOL)\
\
");
            text += std::string("Token Balances:\
");
            auto nonZeroItems = portfolio->items->filter([=](auto item) mutable
            {
                return ((std::make_shared<BigNumber>(item["uiAmount"])))->isGreaterThan(0);
            }
            );
            if (nonZeroItems->length == 0) {
                text += std::string("No tokens found with non-zero balance\
");
            } else {
                for (auto& item : nonZeroItems)
                {
                    auto valueUsd = ((std::make_shared<BigNumber>(item->valueUsd)))->toFixed(2);
                    text += string_empty + item->name + std::string(" (") + item->symbol + std::string("): ") + ((std::make_shared<BigNumber>(item->uiAmount)))->toFixed(6) + std::string(" ($") + valueUsd + std::string(" | ") + item->valueSol + std::string(" SOL)\
");
                }
            }
            if (portfolio->prices) {
                text += std::string("\
Market Prices:\
");
                text += std::string("SOL: $") + values->sol_price + std::string("\
");
                text += std::string("BTC: $") + values->btc_price + std::string("\
");
                text += std::string("ETH: $") + values->eth_price + std::string("\
");
            }
            return object{
                object::pair{std::string("data"), portfolio}, 
                object::pair{std::string("values"), values}, 
                object::pair{std::string("text"), text}
            };
        }
        catch (const any& error)
        {
            console->error(std::string("Error in Solana wallet provider:"), error);
            return object{
                object::pair{std::string("data"), nullptr}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
