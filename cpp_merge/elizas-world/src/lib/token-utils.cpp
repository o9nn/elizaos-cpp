#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-world/src/lib/token-utils.h"

Record<string, double> loadCachedScores()
{
    try
    {
        if (fs->existsSync(SCORES_CACHE_PATH)) {
            auto data = fs->readFileSync(SCORES_CACHE_PATH, std:("utf8"));
            return JSON->parse(data);
        }
    }
    catch (const any& error)
    {
        console->warn(std:("Error loading cached scores:"), error);
    }
    return object{};
};


void saveScoresToCache(Record<string, double> scores)
{
    try
    {
        auto dir = path->dirname(SCORES_CACHE_PATH);
        if (!fs->existsSync(dir)) {
            fs->mkdirSync(dir, object{
                object::pair{std:("recursive"), true}
            });
        }
        fs->writeFileSync(SCORES_CACHE_PATH, JSON->stringify(scores, nullptr, 2));
    }
    catch (const any& error)
    {
        console->warn(std:("Error saving scores to cache:"), error);
    }
};


std::shared_ptr<Promise<array<std::shared_ptr<TokenBalance>>>> getTokenBalances(std::shared_ptr<Connection> connection, std::shared_ptr<PublicKey> walletPubkey)
{
    try
    {
        auto accounts = std::async([=]() { connection->getParsedTokenAccountsByOwner(walletPubkey, object{
            object::pair{std:("programId"), TOKEN_PROGRAM_ID}
        }); });
        return accounts->value->map([=](auto account) mutable
        {
            return (object{
                object::pair{std:("mint"), account["account"]->data->parsed->info->mint}, 
                object::pair{std:("tokenAmount"), account["account"]->data->parsed->info->tokenAmount}
            });
        }
        )->filter([=](auto info) mutable
        {
            return Number(info["tokenAmount"]->amount) > 0;
        }
        );
    }
    catch (const any& error)
    {
        console->error(std:("Error in getTokenBalances:"), (is<Error>(error)) ? error->message : String(error));
        return array<any>();
    }
};


std::shared_ptr<Promise<std::shared_ptr<DexScreenerResponse>>> fetchDexScreenerData(array<string> tokenAddresses)
{
    try
    {
        auto validAddresses = tokenAddresses->filter([=](auto addr) mutable
        {
            return AND((addr), (addr->get_length() > 0));
        }
        );
        if (validAddresses->get_length() == 0) {
            return object{
                object::pair{std:("pairs"), array<any>()}
            };
        }
        auto BATCH_SIZE = 30;
        auto allPairs = array<any>();
        for (auto i = 0; i < validAddresses->get_length(); i += BATCH_SIZE)
        {
            auto batchAddresses = validAddresses->slice(i, i + BATCH_SIZE);
            auto url = std:("https://api.dexscreener.com/latest/dex/tokens/") + batchAddresses->join(std:(",")) + string_empty;
            try
            {
                auto response = std::async([=]() { axios->get(url, object{
                    object::pair{std:("headers"), object{
                        object::pair{std:("Cache-Control"), std:("no-cache")}, 
                        object::pair{std:("Pragma"), std:("no-cache")}
                    }}
                }); });
                if (response->data->pairs) {
                    allPairs->push(const_(response->data->pairs)[0]);
                }
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 200);
                }
                ); });
            }
            catch (const any& error)
            {
                console->error(std:("Error fetching batch ") + (i / BATCH_SIZE + 1) + std:(":"), error);
            }
        }
        return object{
            object::pair{std:("pairs"), allPairs}
        };
    }
    catch (const any& error)
    {
        console->error(std:("Error fetching market data:"), error);
        return object{
            object::pair{std:("pairs"), array<any>()}
        };
    }
};


any fetchTokenAnalysis(string address)
{
    try
    {
        auto response = std::async([=]() { axios->get(std:("https://api.dexscreener.com/latest/dex/pairs/solana/") + address + string_empty); });
        shared pair = const_(response->data->pairs)[0];
        return object{
            object::pair{std:("priceChange24h"), parseFloat(OR((pair->priceChange24h), (std:("0"))))}, 
            object::pair{std:("priceChange7d"), parseFloat(OR((pair->priceChange7d), (std:("0"))))}, 
            object::pair{std:("volumeChange24h"), parseFloat(OR((pair->volumeChange24h), (std:("0"))))}, 
            object::pair{std:("volumeAvg24h"), OR((pair->volume->h24), (0))}, 
            object::pair{std:("txCount24h"), OR((pair->txns->h24), (0))}, 
            object::pair{std:("holders"), OR((pair->holders), (undefined))}, 
            object::pair{std:("timeSeries"), OR((pair->priceUsd->map([=](auto price, auto index) mutable
            {
                return (object{
                    object::pair{std:("timestamp"), Date->now() - (index * 3600000)}, 
                    object::pair{std:("price"), std:("price")}, 
                    object::pair{std:("volume"), pair->volume->h24 / 24}
                });
            }
            )), (array<any>()))}
        };
    }
    catch (const any& error)
    {
        console->error(std:("Error fetching analysis for ") + address + std:(":"), (is<Error>(error)) ? error->message : String(error));
        return nullptr;
    }
};


std::shared_ptr<Promise<array<std::shared_ptr<TokenHolding>>>> calculateHoldings(std::shared_ptr<Connection> connection, array<std::shared_ptr<TokenBalance>> balances, std::shared_ptr<DexScreenerResponse> marketData, string walletAddress)
{
    auto holdings = array<std::shared_ptr<TokenHolding>>();
    auto seenPairs = std::make_shared<Set<string>>();
    if (!marketData->pairs) {
        return holdings;
    }
    auto tokenAddresses = marketData->pairs->map([=](auto pair) mutable
    {
        return pair["baseToken"]->address;
    }
    )->filter([=](auto addr) mutable
    {
        return !!addr;
    }
    );
    console->log(std:("Fetching scores for"), tokenAddresses->length, std:("tokens"));
    auto scores = std::async([=]() { fetchTokenScores(tokenAddresses); });
    console->log(std:("Fetched scores:"), scores);
    for (auto& pair : marketData->pairs)
    {
        if (OR((!pair->baseToken->address), (seenPairs->has(pair->baseToken->address)))) {
            continue;
        }
        seenPairs->add(pair->baseToken->address);
        auto balance = balances->find([=](auto b) mutable
        {
            return b->mint == pair->baseToken->address;
        }
        );
        if (balance) {
            auto totalSupply = (pair->fdv) ? any(pair->fdv / Number(pair->priceUsd)) (undefined);
            auto percentageOwned = (totalSupply) ? any((balance->tokenAmount->uiAmount / totalSupply) * 100) (0);
            auto usdValue = balance->tokenAmount->uiAmount * Number(pair->priceUsd);
            holdings->push(object{
                object::pair{std:("address"), pair->baseToken->address}, 
                object::pair{std:("balance"), balance->tokenAmount->uiAmount}, 
                object::pair{std:("decimals"), balance->tokenAmount->decimals}, 
                object::pair{std:("usdValue"), std:("usdValue")}, 
                object::pair{std:("percentageOwned"), std:("percentageOwned")}, 
                object::pair{std:("firstReceived"), 0}, 
                object::pair{std:("marketData"), utils::assign(object{
                    , 
                    object::pair{std:("score"), (*const_(scores))[pair->baseToken->address]}
                }, pair)}
            });
        }
    }
    console->log(std:("Final holdings with scores:"), holdings->map([=](auto h) mutable
    {
        return (object{
            object::pair{std:("address"), h->address}, 
            object::pair{std:("score"), h->marketData->score}
        });
    }
    ));
    return holdings->sort([=](auto a, auto b) mutable
    {
        return b->usdValue - a->usdValue;
    }
    );
};


any getTokenData(std::shared_ptr<Connection> connection)
{
    try
    {
        auto walletPubkey = std::make_shared<PublicKey>(std:("AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG"));
        auto balances = std::async([=]() { getTokenBalances(connection, walletPubkey); });
        return balances;
    }
    catch (const any& error)
    {
        console->error(std:("Error in getTokenData:"), (is<Error>(error)) ? error->message : String(error));
        return array<any>();
    }
};


std::shared_ptr<Promise<Record<string, double>>> fetchTokenScores(array<string> tokenAddresses)
{
    shared cachedScores = loadCachedScores();
    auto uncachedAddresses = tokenAddresses->filter([=](auto addr) mutable
    {
        return AND((AND((addr), (addr->get_length() > 0))), ((*const_(cachedScores))[addr] == undefined));
    }
    );
    if (uncachedAddresses->get_length() == 0) {
        return cachedScores;
    }
    try
    {
        auto BATCH_SIZE = 20;
        shared newScores = object{};
        for (auto i = 0; i < uncachedAddresses->get_length(); i += BATCH_SIZE)
        {
            auto batchAddresses = uncachedAddresses->slice(i, i + BATCH_SIZE);
            try
            {
                console->log(std:("Fetching scores for batch ") + (i / BATCH_SIZE + 1) + std:(", addresses:"), batchAddresses);
                auto response = std::async([=]() { axios->post<std::shared_ptr<SolSnifferResponse>>(std:("https://solsniffer.com/api/v2/tokens"), object{
                    object::pair{std:("addresses"), batchAddresses}
                }, object{
                    object::pair{std:("headers"), object{
                        object::pair{std:("accept"), std:("application/json")}, 
                        object::pair{std:("X-API-KEY"), std:("891aayu3sa4lbg4m8gu9gtfct3pxcp")}, 
                        object::pair{std:("Content-Type"), std:("application/json")}
                    }}, 
                    object::pair{std:("timeout"), 10000}
                }); });
                if (response->data->data) {
                    response->data->data->forEach([=](auto token) mutable
                    {
                        if (AND((token["address"]), (token["tokenData"]["score"] != undefined))) {
                            newScores[token["address"]] = token["tokenData"]["score"];
                        }
                    }
                    );
                }
                console->log(std:("Batch ") + (i / BATCH_SIZE + 1) + std:(" results:"), response->data->data->map([=](auto t) mutable
                {
                    return (object{
                        object::pair{std:("address"), t["address"]}, 
                        object::pair{std:("score"), t["tokenData"]->score}, 
                        object::pair{std:("error"), t["error"]}
                    });
                }
                ));
                if (i + BATCH_SIZE < uncachedAddresses->get_length()) {
                    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        return setTimeout(resolve, 1000);
                    }
                    ); });
                }
            }
            catch (const any& batchError)
            {
                console->warn(std:("Failed to fetch scores for batch ") + (i / BATCH_SIZE + 1) + std:(":"), (is<Error>(batchError)) ? batchError->message : String(batchError));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 2000);
                }
                ); });
                continue;
            }
        }
        auto allScores = utils::assign(object{
        }, cachedScores, newScores);
        saveScoresToCache(allScores);
        return allScores;
    }
    catch (const any& error)
    {
        console->error(std:("Error in fetchTokenScores:"), (is<Error>(error)) ? error->message : String(error));
        return cachedScores;
    }
};


object config = object{
    object::pair{std:("apiKey"), OR((process->env->NEXT_PUBLIC_ALCHEMY_API_KEY), (std:("7CBPP2HmBAKkdbI4gbO7ruEt_wLCyGQ2")))}, 
    object::pair{std:("network"), Network->ETH_MAINNET}
};
any alchemy = std::make_shared<Alchemy>(config);
any SCORES_CACHE_PATH = path->join(process->cwd(), std:("data"), std:("token-scores.json"));

void Main(void)
{
}

MAIN
