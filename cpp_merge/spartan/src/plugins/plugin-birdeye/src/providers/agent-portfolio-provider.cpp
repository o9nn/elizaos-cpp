#include "agent-portfolio-provider.h"

std::shared_ptr<Provider> agentPortfolioProvider = object{
    object::pair{std:("name"), std:("BIRDEYE_TRADE_PORTFOLIO")}, 
    object::pair{std:("description"), std:("Birdeye intel on agent's walllet")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        try
        {
            auto provider = std::make_shared<BirdeyeProvider>(runtime->cacheManager);
            auto walletAddr = runtime->getSetting(std:("BIRDEYE_WALLET_ADDR"));
            if (!walletAddr) {
                console->warn(std:("No Birdeye wallet was specified"));
                return string_empty;
            }
            auto chain = extractChain(walletAddr);
            auto resp = std::async([=]() { provider->fetchWalletPortfolio(object{
                object::pair{std:("wallet"), walletAddr}
            }, object{
                object::pair{std:("headers"), object{
                    object::pair{std:("chain"), std:("chain")}
                }}
            }); });
            auto portfolioText = formatPortfolio(resp);
            return std:("This is your wallet address: ") + walletAddr + std:("\
\
This is your portfolio: [") + portfolioText + std:("]");
        }
        catch (const any& error)
        {
            console->error(std:("Error fetching token data:"), error);
            return std:("Unable to fetch token information. Please try again later.");
        }
    }
    }
};

void Main(void)
{
}

MAIN
