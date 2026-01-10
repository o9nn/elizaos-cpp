#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/src/providers/agent-portfolio-provider.h"

std::shared_ptr<Provider> agentPortfolioProvider = object{
    object::pair{std::string("name"), std::string("BIRDEYE_TRADE_PORTFOLIO")}, 
    object::pair{std::string("description"), std::string("Birdeye intel on agent's walllet")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        try
        {
            auto provider = std::make_shared<BirdeyeProvider>(runtime->cacheManager);
            auto walletAddr = runtime->getSetting(std::string("BIRDEYE_WALLET_ADDR"));
            if (!walletAddr) {
                console->warn(std::string("No Birdeye wallet was specified"));
                return string_empty;
            }
            auto chain = extractChain(walletAddr);
            auto resp = std::async([=]() { provider->fetchWalletPortfolio(object{
                object::pair{std::string("wallet"), walletAddr}
            }, object{
                object::pair{std::string("headers"), object{
                    object::pair{std::string("chain"), std::string("chain")}
                }}
            }); });
            auto portfolioText = formatPortfolio(resp);
            return std::string("This is your wallet address: ") + walletAddr + std::string("\
\
This is your portfolio: [") + portfolioText + std::string("]");
        }
        catch (const any& error)
        {
            console->error(std::string("Error fetching token data:"), error);
            return std::string("Unable to fetch token information. Please try again later.");
        }
    }
    }
};

void Main(void)
{
}

MAIN
