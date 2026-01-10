#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/providers/birdeyeWallet.h"

std::shared_ptr<Provider> birdeyeTradePortfolioProvider = object{
    object::pair{std::string("name"), std::string("INTEL_TRADE_PORTFOLIO")}, 
    object::pair{std::string("description"), std::string("A list of your trades")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        console->log(std::string("intel:provider - get portfolio"));
        auto chains = array<string>{ std::string("solana"), std::string("base") };
        auto portfolioData = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<Portfolio>>>(std::string("portfolio")); }))), (array<any>()));
        auto portfolio = portfolioData["data"];
        console->log(std::string("intel:provider - got portfolio"), portfolio);
        auto trades = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<TransactionHistory>>>(std::string("transaction_history")); }))), (array<any>()));
        console->log(std::string("intel:provider - got trades"), trades["length"]);
        auto promptInjection = std::string("\
Your trades for ") + portfolio["wallet"] + std::string(" (value: $") + portfolio["totalUsd"] + std::string("usd):\
");
        auto historyStrings = array<any>();
        try
        {
            auto idx = 1;
            auto& __array3956_4830 = trades;
            for (auto __indx3956_4830 = 0_N; __indx3956_4830 < __array3956_4830->get_length(); __indx3956_4830++)
            {
                auto& h = const_(__array3956_4830)[__indx3956_4830];
                {
                    console->log(std::string("h"), h["data"]["status"] == true, !!(AND((h["data"]), (h["data"]["balanceChange"]))), AND((AND((h["data"]), (h["data"]["balanceChange"]))), (h["data"]["balanceChange"]["length"] > 0)));
                    if (AND((AND((AND((h["data"]["status"] == true), (h["data"]))), (h["data"]["balanceChange"]))), (h["data"]["balanceChange"]["length"] > 0))) {
                        console->log(std::string("inside"));
                        auto change = const_(h["data"]["balanceChange"])[0];
                        auto action = OR((h["data"]["mainAction"]), (std::string("unknown action")));
                        auto amount = OR((change["amount"]), (0));
                        auto name = OR((change["name"]), (std::string("unknown")));
                        auto symbol = OR((change["symbol"]), (std::string("?")));
                        auto time = OR((h["blockTime"]), (std::string("unknown time")));
                        auto summary = string_empty + action + std::string(" ") + amount + std::string(" ") + name + std::string(" ($") + symbol + std::string(") at ") + time + string_empty;
                        historyStrings->push(summary);
                    }
                }
            }
        }
        catch (const any& e)
        {
            console->error(std::string("e"), e);
        }
        promptInjection += historyStrings->join(std::string("\
")) + std::string("\
");
        auto data = object{
            object::pair{std::string("portfolio"), std::string("portfolio")}, 
            object::pair{std::string("trades"), std::string("trades")}
        };
        auto values = object{};
        auto text = promptInjection + std::string("\
");
        return object{
            object::pair{std::string("data"), std::string("data")}, 
            object::pair{std::string("values"), std::string("values")}, 
            object::pair{std::string("text"), std::string("text")}
        };
        return false;
    }
    }
};

void Main(void)
{
}

MAIN
