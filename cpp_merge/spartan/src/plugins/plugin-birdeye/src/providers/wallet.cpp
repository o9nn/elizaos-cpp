#include "wallet.hpp"

std::shared_ptr<Provider> tradePortfolioProvider = object{
    object::pair{std:("name"), std:("BIRDEYE_TRADE_PORTFOLIO")}, 
    object::pair{std:("description"), std:("A list of your trades")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state) mutable
    {
        console->log(std:("birdeye:provider - get portfolio"));
        auto chains = array<string>{ std:("solana"), std:("base") };
        auto portfolioData = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<Portfolio>>>(std:("portfolio")); }))), (array<any>()));
        auto portfolio = portfolioData["data"];
        auto trades = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<TransactionHistory>>>(std:("transaction_history")); }))), (array<any>()));
        console->log(std:("birdeye:provider - got trades"), trades["length"]);
        auto promptInjection = std:("\
Your trades for ") + portfolio["wallet"] + std:(" (value: $") + portfolio["totalUsd"] + std:("usd):\
");
        auto historyStrings = array<any>();
        try
        {
            auto idx = 1;
            auto& __array3965_4663 = trades;
            for (auto __indx3965_4663 = 0_N; __indx3965_4663 < __array3965_4663->get_length(); __indx3965_4663++)
            {
                auto& h = const_(__array3965_4663)[__indx3965_4663];
                {
                    if (AND((AND((h["data"]["status"] == true), (h["data"]["balanceChange"]))), (h["data"]["balanceChange"]["length"] > 0))) {
                        auto change = const_(h["data"]["balanceChange"])[0];
                        auto action = OR((h["data"]["mainAction"]), (std:("unknown action")));
                        auto amount = OR((change["amount"]), (0));
                        auto name = OR((change["name"]), (std:("unknown")));
                        auto symbol = OR((change["symbol"]), (std:("?")));
                        auto time = OR((h["blockTime"]), (std:("unknown time")));
                        auto summary = string_empty + action + std:(" ") + amount + std:(" ") + name + std:(" ($") + symbol + std:(") at ") + time + string_empty;
                    }
                }
            }
        }
        catch (const any& e)
        {
            console->error(std:("e"), e);
        }
        promptInjection += historyStrings->join(std:("\
")) + std:("\
");
        auto data = object{
            object::pair{std:("portfolio"), std:("portfolio")}, 
            object::pair{std:("trades"), std:("trades")}
        };
        auto values = object{};
        auto text = promptInjection + std:("\
");
        return object{
            object::pair{std:("data"), std:("data")}, 
            object::pair{std:("values"), std:("values")}, 
            object::pair{std:("text"), std:("text")}
        };
        return false;
    }
    }
};

void Main(void)
{
}

MAIN
