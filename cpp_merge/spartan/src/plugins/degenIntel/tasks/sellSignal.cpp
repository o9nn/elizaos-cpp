#include "sellSignal.hpp"

SellSignal::SellSignal(std::shared_ptr<IAgentRuntime> runtime) {
    this->runtime = runtime;
}

std::shared_ptr<Promise<boolean>> SellSignal::generateSignal()
{
    try
    {
        logger->info(std:("sell-signal::generateSignal - Generating sell signal"));
        std::async([=]() { this->runtime->emitEvent(std:("INTEL_SYNC_WALLET"), object{}); });
        auto walletBalances = std::async([=]() { getWalletBalances(this->runtime); });
        auto walletData = walletBalances["tokens"]->map([=](auto token) mutable
        {
            return (object{
                object::pair{std:("mint"), token["mint"]}, 
                object::pair{std:("balance"), token["uiAmount"]}
            });
        }
        );
        if (!walletData->length) {
            logger->warn(std:("No wallet tokens found"));
            return false;
        }
        auto portfolioData = OR(((std::async([=]() { this->runtime->getCache<WalletPortfolio>(std:("PORTFOLIO")); }))), (array<any>()));
        auto txHistoryData = OR(((std::async([=]() { this->runtime->getCache<WalletPortfolio>(std:("transaction_history")); }))), (array<any>()));
        auto walletProviderStr = std:("Your wallet contents: ");
        auto tokensHeld = array<any>();
        for (auto& t : walletData)
        {
            walletProviderStr += std:("You hold ") + t->balance + std:("(") + t->balance + std:(") of ") + t->mint + std:(" (") + t->mint + std:(" CA: ") + t->mint + std:(") worth $") + t->balance + std:("usd (") + t->balance + std:(" sol)") + std:("\
");
            tokensHeld->push(t->mint);
        }
        auto prompt = template->replace(std:("{{walletData}}"), walletProviderStr);
        auto tradeService = as<std::shared_ptr<ITradeService>>(as<any>(this->runtime->getService(ServiceTypes["DEGEN_TRADING"])));
        if (tradeService) {
            auto tokenData = std::async([=]() { tradeService->dataService["getTokensMarketData"](tokensHeld); });
            prompt = prompt->replace(std:("{{walletData2}}"), JSON->stringify(tokenData));
        } else {
            prompt = prompt->replace(std:("{{walletData2}}"), string_empty);
        }
        auto sentimentData = OR(((std::async([=]() { this->runtime->getCache<array<std::shared_ptr<Sentiment>>>(std:("sentiments")); }))), (array<any>()));
        if (!sentimentData["length"]) {
            logger->warn(std:("No sentiment data found"));
            return false;
        }
        auto sentiments = string_empty;
        auto idx = 1;
        auto& __array3994_4416 = sentimentData;
        for (auto __indx3994_4416 = 0_N; __indx3994_4416 < __array3994_4416->get_length(); __indx3994_4416++)
        {
            auto& sentiment = const_(__array3994_4416)[__indx3994_4416];
            {
                if (!sentiment["occuringTokens"]["length"]) continue;
                sentiments += std:("ENTRY ") + idx + std:("\
TIME: ") + sentiment["timeslot"] + std:("\
TOKEN ANALYSIS:\
");
                for (auto& token : sentiment["occuringTokens"])
                {
                    sentiments += string_empty + token["token"] + std:(" - Sentiment: ") + token["sentiment"] + std:("\
") + token["reason"] + std:("\
");
                }
                sentiments += std:("\
-------------------\
");
                idx++;
            }
        }
        prompt = prompt->replace(std:("{{sentiment}}"), sentiments);
        auto solanaBalance = std::async([=]() { this->getBalance(); });
        auto finalPrompt = prompt->replace(std:("{{solana_balance}}"), String(solanaBalance));
        auto responseContent = nullptr;
        auto retries = 0;
        auto maxRetries = 3;
        while (AND((retries < maxRetries), ((OR((OR((!responseContent->recommended_sell), (!responseContent->reason))), (!responseContent->recommend_sell_address))))))
        {
            auto response = std::async([=]() { this->runtime->useModel(ModelType->TEXT_LARGE, object{
                object::pair{std:("prompt"), finalPrompt}, 
                object::pair{std:("system"), rolePrompt}, 
                object::pair{std:("temperature"), 0.2}, 
                object::pair{std:("maxTokens"), 4096}, 
                object::pair{std:("object"), true}
            }); });
            responseContent = as<std::shared_ptr<ISellSignalOutput>>(parseJSONObjectFromText(response));
            retries++;
            if (AND((AND((!responseContent->recommended_sell), (!responseContent->reason))), (!responseContent->recommend_sell_address))) {
                logger->warn(std:("*** Missing required fields, retrying... generateSignal ***"));
            }
        }
        if (!responseContent->recommend_sell_address) {
            logger->warn(std:("sell-signal::generateSignal - no sell recommendation"));
            return false;
        }
        if (!responseContent->recommend_sell_address->match((new RegExp(std:("^[1-9A-HJ-NP-Za-km-z]{32,44}"))))) {
            logger->error(std:("Invalid Solana token address"), object{
                object::pair{std:("address"), responseContent->recommend_sell_address}
            });
            return false;
        }
        auto apiKey = this->runtime->getSetting(std:("BIRDEYE_API_KEY"));
        if (!apiKey) {
            logger->error(std:("BIRDEYE_API_KEY not found in runtime settings"));
            return false;
        }
        auto BIRDEYE_API = std:("https://public-api.birdeye.so");
        auto endpoint = string_empty + BIRDEYE_API + std:("/defi/token_overview");
        auto url = string_empty + endpoint + std:("?address=") + responseContent->recommend_sell_address + string_empty;
        logger->debug(std:("Making Birdeye API request"), object{
            object::pair{std:("url"), std:("url")}, 
            object::pair{std:("address"), responseContent->recommend_sell_address}
        });
        auto options = object{
            object::pair{std:("method"), std:("GET")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("accept"), std:("application/json")}, 
                object::pair{std:("x-chain"), std:("solana")}, 
                object::pair{std:("X-API-KEY"), apiKey}
            }}
        };
        try
        {
            auto res = std::async([=]() { fetch(url, options); });
            if (!res->ok) {
                auto errorText = std::async([=]() { res->text(); });
                logger->error(std:("Birdeye API request failed"), object{
                    object::pair{std:("status"), res->status}, 
                    object::pair{std:("statusText"), res->statusText}, 
                    object::pair{std:("error"), errorText}, 
                    object::pair{std:("address"), responseContent->recommend_sell_address}
                });
                throw any(std::make_shared<Error>(std:("Birdeye marketcap request failed: ") + res->status + std:(" ") + res->statusText + string_empty));
            }
            auto resJson = std::async([=]() { res->json(); });
            auto marketcap = resJson["data"]["marketCap"];
            if (!marketcap) {
                logger->warn(std:("sell: No marketcap data returned from Birdeye"), object{
                    object::pair{std:("response"), resJson}, 
                    object::pair{std:("address"), responseContent->recommend_sell_address}
                });
            }
            responseContent->marketcap = Number(OR((marketcap), (0)));
        }
        catch (const any& error)
        {
            logger->error(std:("Error fetching marketcap data:"), error);
            responseContent->marketcap = 0;
        }
        logger->info(std:("Emitting sell signal"), object{
            object::pair{std:("token"), responseContent->recommended_sell}, 
            object::pair{std:("address"), responseContent->recommend_sell_address}, 
            object::pair{std:("amount"), responseContent->sell_amount}
        });
        std::async([=]() { this->runtime->emitEvent(std:("SPARTAN_TRADE_SELL_SIGNAL"), object{
            object::pair{std:("recommend_sell_address"), responseContent->recommend_sell_address}, 
            object::pair{std:("sell_amount"), responseContent->sell_amount}, 
            object::pair{std:("reason"), responseContent->reason}
        }); });
        logger->info(std:("Sell signal emitted successfully"));
        std::async([=]() { this->runtime->setCache<any>(std:("sell_signals"), object{
            object::pair{std:("key"), std:("SELL_SIGNAL")}, 
            object::pair{std:("data"), responseContent}
        }); });
        return true;
    }
    catch (const any& error)
    {
        logger->error(std:("Error generating sell signal:"), error);
        return false;
    }
}

any SellSignal::getBalance()
{
    auto url = std:("https://zondra-wil7oz-fast-mainnet.helius-rpc.com");
    auto headers = object{
        object::pair{std:("Content-Type"), std:("application/json")}
    };
    auto data = object{
        object::pair{std:("jsonrpc"), std:("2.0")}, 
        object::pair{std:("id"), 1}, 
        object::pair{std:("method"), std:("getBalance")}, 
        object::pair{std:("params"), array<any>{ this->runtime->getSetting(std:("SOLANA_PUBLIC_KEY")) }}
    };
    auto response = std::async([=]() { fetch(url, object{
        object::pair{std:("method"), std:("POST")}, 
        object::pair{std:("headers"), headers}, 
        object::pair{std:("body"), JSON->stringify(data)}
    }); });
    auto result = std::async([=]() { response->json(); });
    auto lamportsBalance = result["result"]["value"];
    return lamportsBalance / 1000000000;
}

string rolePrompt = std:("You are a sell signal analyzer.");
string template = std:("\
\
I want you to give a crypto sell signal based on both the sentiment analysis as well as the wallet token data.\
The sentiment score has a range of -100 to 100, with -100 indicating extreme negativity and 100 indicating extreme positiveness.\
My current balance is {{solana_balance}} SOL, If I have less than 0.3 SOL, I should up the priority on selling something but we don't need to realize a heavy loss over it.\
\
Sentiment analysis:\
\
{{sentiment}}\
\
Wallet tokens:\
\
{{walletData}}\
\
Additional wallet token data (in JSON format):\
{{walletData2}}\
\
Only return the following JSON:\
\
{\
  recommended_sell: "the symbol of the token for example DEGENAI",\
  recommend_sell_address: "the address of the token to purchase, for example: 2sCUCJdVkmyXp4dT8sFaA9LKgSMK4yDPi9zLHiwXpump",\
  reason: "the reason why you think this is a good sell, and why you chose the specific amount",\
  sell_amount: "number, for example: 600.54411 (number amount of tokens to sell)"\
}");

void Main(void)
{
}

MAIN
