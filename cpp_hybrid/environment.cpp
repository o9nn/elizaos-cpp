#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-coinmarketcap/src/environment.h"

std::shared_ptr<Promise<CoinMarketCapConfig>> validateCoinMarketCapConfig(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        auto config = object{
            object::pair{std::string("COINMARKETCAP_API_KEY"), runtime->getSetting(std::string("COINMARKETCAP_API_KEY"))}
        };
        return coinmarketcapEnvSchema->parse(config);
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            auto errorMessages = error["errors"]["map"]([=](auto err) mutable
            {
                return string_empty + err["path"]->join(std::string(".")) + std::string(": ") + err["message"] + string_empty;
            }
            )["join"](std::string("\
"));
            throw any(std::make_shared<Error>(std::string("CoinMarketCap configuration validation failed:\
") + errorMessages + string_empty));
        }
        throw any(error);
    }
};


any coinmarketcapEnvSchema = z->object(object{
    object::pair{std::string("COINMARKETCAP_API_KEY"), z->string()->min(1, std::string("CoinMarketCap API key is required"))}
});

void Main(void)
{
}

MAIN
