#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-world/src/lib/token-utils.h"

Record<string, double> loadCachedScores()
{
    try
    {
        if (fs->existsSync(SCORES_CACHE_PATH)) {
            auto data = fs->readFileSync(SCORES_CACHE_PATH, std::string("utf8"));
            return JSON->parse(data);
        }
    }
    catch (const any& error)
    {
        console->warn(std::string("Error loading cached scores:"), error);
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
                object::pair{std::string("recursive"), true}
            });
        }
        fs->writeFileSync(SCORES_CACHE_PATH, JSON->stringify(scores, nullptr, 2));
    }
    catch (const any& error)
    {
        console->warn(std::string("Error saving scores to cache:"), error);
    }
};


std::shared_ptr<Promise<array<std::shared_ptr<TokenBalance>>>> getTokenBalances(std::shared_ptr<Connection> connection, std::shared_ptr<PublicKey> walletPubkey)
{
    try
    {
        auto accounts = std::async([=]() { connection->getParsedTokenAccountsByOwner(walletPubkey, object{
            object::pair{std::string("programId"), TOKEN_PROGRAM_ID}
        }); });
        return accounts->value->map([=](auto account) mutable
        {
            return (object{
                object::pair{std::string("mint"), account["account"]->data->parsed->info->mint}, 
                object::pair{std::string("tokenAmount"), account["account"]->data->parsed->info->tokenAmount}
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
        console->error(std::string("Error in getTokenBalances:"), (is<Error>(error)) ? error->message : String(error));
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
                object::pair{std::string("pairs"), array<any>()}
            };
        }
        auto BATCH_SIZE = 30;
        auto allPairs = array<any>();
        for (auto i = 0; i < validAddresses->get_length(); i += BATCH_SIZE)
        {
            auto batchAddresses = validAddresses->slice(i, i + BATCH_SIZE);
            auto url = std::string("https://api.dexscreener.com/latest/dex/tokens/") + batchAddresses->join(std::string(",")) + string_empty;
            try
            {
                auto response = std::async([=]() { axios->get(url, object{
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("Cache-Control"), std::string("no-cache")}, 
                        object::pair{std::string("Pragma"), std::string("no-cache")}
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
                console->error(std::string("Error fetching batch ") + (i / BATCH_SIZE + 1) + std::string(":"), error);
            }
        }
        return object{
            object::pair{std::string("pairs"), allPairs}
        };
    }
    catch (const any& error)
    {
        console->error(std::string("Error fetching market data:"), error);
        return object{
            object::pair{std::string("pairs"), array<any>()}
        };
    }
};


any fetchTokenAnalysis(string address)
{
    try
    {
        auto response = std::async([=]() { axios->get(std::string("https://api.dexscreener.com/latest/dex/pairs/solana/") + address + string_empty); });
        shared pair = const_(response->data->pairs)[0];
        return object{
            object::pair{std::string("priceChange24h"), parseFloat(OR((pair->priceChange24h), (std::string("0"))))}, 
            object::pair{std::string("priceChange7d"), parseFloat(OR((pair->priceChange7d), (std::string("0"))))}, 
            object::pair{std::string("volumeChange24h"), parseFloat(OR((pair->volumeChange24h), (std::string("0"))))}, 
            object::pair{std::string("volumeAvg24h"), OR((pair->volume->h24), (0))}, 
            object::pair{std::string("txCount24h"), OR((pair->txns->h24), (0))}, 
            object::pair{std::string("holders"), OR((pair->holders), (undefined))}, 
            object::pair{std::string("timeSeries"), OR((pair->priceUsd->map([=](auto price, auto index) mutable
            {
                return (object{
                    object::pair{std::string("timestamp"), Date->now() - (index * 3600000)}, 
                    object::pair{std::string("price"), std::string("price")}, 
                    object::pair{std::string("volume"), pair->volume->h24 / 24}
                });
            }
            )), (array<any>()))}
        };
    }
    catch (const any& error)
    {
        console->error(std::string("Error fetching analysis for ") + address + std::string(":"), (is<Error>(error)) ? error->message : String(error));
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
    console->log(std::string("Fetching scores for"), tokenAddresses->length, std::string("tokens"));
    auto scores = std::async([=]() { fetchTokenScores(tokenAddresses); });
    console->log(std::string("Fetched scores:"), scores);
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
            auto totalSupply = (pair->fdv) ? any(pair->fdv / Number(pair->priceUsd)) : any(undefined);
            auto percentageOwned = (totalSupply) ? any((balance->tokenAmount->uiAmount / totalSupply) * 100) : any(0);
            auto usdValue = balance->tokenAmount->uiAmount * Number(pair->priceUsd);
            holdings->push(object{
                object::pair{std::string("address"), pair->baseToken->address}, 
                object::pair{std::string("balance"), balance->tokenAmount->uiAmount}, 
                object::pair{std::string("decimals"), balance->tokenAmount->decimals}, 
                object::pair{std::string("usdValue"), std::string("usdValue")}, 
                object::pair{std::string("percentageOwned"), std::string("percentageOwned")}, 
                object::pair{std::string("firstReceived"), 0}, 
                object::pair{std::string("marketData"), utils::assign(object{
                    , 
                    object::pair{std::string("score"), (*const_(scores))[pair->baseToken->address]}
                }, pair)}
            });
        }
    }
    console->log(std::string("Final holdings with scores:"), holdings->map([=](auto h) mutable
    {
        return (object{
            object::pair{std::string("address"), h->address}, 
            object::pair{std::string("score"), h->marketData->score}
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
        auto walletPubkey = std::make_shared<PublicKey>(std::string("AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG"));
        auto balances = std::async([=]() { getTokenBalances(connection, walletPubkey); });
        return balances;
    }
    catch (const any& error)
    {
        console->error(std::string("Error in getTokenData:"), (is<Error>(error)) ? error->message : String(error));
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
                console->log(std::string("Fetching scores for batch ") + (i / BATCH_SIZE + 1) + std::string(", addresses:"), batchAddresses);
                auto response = std::async([=]() { axios->post<std::shared_ptr<SolSnifferResponse>>(std::string("https://solsniffer.com/api/v2/tokens"), object{
                    object::pair{std::string("addresses"), batchAddresses}
                }, object{
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("accept"), std::string("application/json")}, 
                        object::pair{std::string("X-API-KEY"), std::string("891aayu3sa4lbg4m8gu9gtfct3pxcp")}, 
                        object::pair{std::string("Content-Type"), std::string("application/json")}
                    }}, 
                    object::pair{std::string("timeout"), 10000}
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
                console->log(std::string("Batch ") + (i / BATCH_SIZE + 1) + std::string(" results:"), response->data->data->map([=](auto t) mutable
                {
                    return (object{
                        object::pair{std::string("address"), t["address"]}, 
                        object::pair{std::string("score"), t["tokenData"]->score}, 
                        object::pair{std::string("error"), t["error"]}
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
                console->warn(std::string("Failed to fetch scores for batch ") + (i / BATCH_SIZE + 1) + std::string(":"), (is<Error>(batchError)) ? batchError->message : String(batchError));
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
        console->error(std::string("Error in fetchTokenScores:"), (is<Error>(error)) ? error->message : String(error));
        return cachedScores;
    }
};


object config = object{
    object::pair{std::string("apiKey"), OR((process->env->NEXT_PUBLIC_ALCHEMY_API_KEY), (std::string("7CBPP2HmBAKkdbI4gbO7ruEt_wLCyGQ2")))}, 
    object::pair{std::string("network"), Network->ETH_MAINNET}
};
any alchemy = std::make_shared<Alchemy>(config);
any SCORES_CACHE_PATH = path->join(process->cwd(), std::string("data"), std::string("token-scores.json"));

void Main(void)
{
}

MAIN
