#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/routes/swap.h"

any codex = std::make_shared<Codex>(process->env->CODEX_API_KEY);
any router = std::make_shared<Hono<object>>();
any ChartParamsSchema = z->object(object{
    object::pair{std::string("pairIndex"), z->string()->transform([=](auto val) mutable
    {
        return parseInt(val);
    }
    )}, 
    object::pair{std::string("start"), z->string()->transform([=](auto val) mutable
    {
        return parseInt(val);
    }
    )}, 
    object::pair{std::string("end"), z->string()->transform([=](auto val) mutable
    {
        return parseInt(val);
    }
    )}, 
    object::pair{std::string("range"), z->string()->transform([=](auto val) mutable
    {
        return parseInt(val);
    }
    )}, 
    object::pair{std::string("token"), z->string()->min(32)->max(44)}
});

void Main(void)
{
    if (!process->env->CODEX_API_KEY) {
        logger["error"](std::string("Missing CODEX_API_KEY from .env"));
        process->exit(1);
    }
    router->get(std::string("/chart/:pairIndex/:start/:end/:range/:token"), honoCacheMiddleware(object{
        object::pair{std::string("cacheName"), std::string("chart-cache")}, 
        object::pair{std::string("cacheControl"), std::string("max-age=120")}, 
        object::pair{std::string("wait"), true}
    }), [=](auto c) mutable
    {
        try
        {
            auto params = ChartParamsSchema->parse(c["req"]["param"]());
            auto redisCache = std::async([=]() { getGlobalRedisCache(); });
            auto from = moment()->utc()->subtract(30, std::string("minutes"))->startOf(std::string("minute"))->unix();
            auto to = moment()->utc()->endOf(std::string("minute"))->unix();
            auto cacheKey = string_empty + from + std::string(":") + to + std::string(":1:") + params->token + string_empty;
            auto cache = std::async([=]() { redisCache->getCompressed(cacheKey); });
            if (cache) {
                return c["json"](cache);
            }
            auto data = std::async([=]() { fetchPriceChartData(from, to, 1, params->token); });
            std::async([=]() { redisCache->setCompressed(cacheKey, JSON->stringify(object{
                object::pair{std::string("table"), data}
            }), 8); });
            return c["json"](object{
                object::pair{std::string("table"), data}
            });
        }
        catch (const any& error)
        {
            if (is<z->ZodError>(error)) {
                c["json"](object{
                    object::pair{std::string("error"), error["errors"]}
                }, 400);
            } else {
                logger["error"](error);
                c["json"](object{
                    object::pair{std::string("error"), std::string("Internal server error")}
                }, 500);
            }
        }
    }
    );
    router->post(std::string("/creator-tokens"), [=](auto c) mutable
    {
        auto user = c["get"](std::string("user"));
        if (!user) {
            return c["json"](object{
                object::pair{std::string("message"), std::string("Unauthorized")}
            }, 401);
        }
        auto db = getDB();
        auto tokensCreated = std::async([=]() { db["select"]()["from"](tokens)["where"](and(eq(tokens->creator, user["publicKey"]), eq(tokens->imported, 0)))["orderBy"](desc(tokens->createdAt)); });
        return c["json"](object{
            object::pair{std::string("tokens"), tokensCreated}
        });
    }
    );
    router->get(std::string("/swaps/:mint"), [=](auto c) mutable
    {
        try
        {
            auto mint = c["req"]["param"](std::string("mint"));
            if (OR((OR((!mint), (mint["length"] < 32))), (mint["length"] > 44))) {
                return c["json"](object{
                    object::pair{std::string("error"), std::string("Invalid mint address")}
                }, 400);
            }
            auto limit = 50;
            auto redisCache = std::async([=]() { getGlobalRedisCache(); });
            auto cacheKey = std::string("swapsListCompressed:") + mint + string_empty;
            auto lookUpKey = std::string("swapsList:") + mint + string_empty;
            auto cache = std::async([=]() { redisCache->getCompressed(cacheKey); });
            if (cache) {
                return c["json"](cache);
            }
            auto responseData = object{};
            auto db = getDB();
            auto token = const_((std::async([=]() { db["select"](object{
                object::pair{std::string("imported"), tokens->imported}, 
                object::pair{std::string("status"), tokens->status}
            })["from"](tokens)["where"](eq(tokens->mint, mint))["limit"](1); })))[0];
            if (!token) throw any(std::make_shared<Error>(std::string("Token not found")));
            auto isCodex = useCodex(token);
            if (isCodex) {
                shared data = std::async([=]() { codex->queries->getTokenEvents(object{
                    object::pair{std::string("query"), object{
                        object::pair{std::string("address"), mint}, 
                        object::pair{std::string("networkId"), 1399811149}, 
                        object::pair{std::string("eventType"), EventType->Swap}
                    }}, 
                    object::pair{std::string("direction"), RankingDirection->Desc}, 
                    object::pair{std::string("limit"), std::string("limit")}
                }); });
                auto items = data->getTokenEvents->items;
                responseData = object{
                    object::pair{std::string("swaps"), items->map([=](auto swap) mutable
                    {
                        return object{
                            object::pair{std::string("account"), OR((swap["maker"]), (std::string("NA")))}, 
                            object::pair{std::string("swapType"), OR((swap["eventDisplayType"]), (std::string("Buy")))}, 
                            object::pair{std::string("solana"), OR((swap["data"]["priceBaseTokenTotal"]), (std::string("0")))}, 
                            object::pair{std::string("tokenAmount"), OR((swap["data"]["amountNonLiquidityToken"]), (std::string("0")))}, 
                            object::pair{std::string("transactionHash"), OR((swap["transactionHash"]), (string_empty))}, 
                            object::pair{std::string("timestamp"), OR((swap["timestamp"] * 1000), (0))}, 
                            object::pair{std::string("usdValue"), OR((swap["data"]["priceUsdTotal"]), (nullptr))}
                        };
                    }
                    )}
                };
            } else {
                auto swapsResultRaw = array<any>();
                auto [swapStrings] = std::async([=]() { Promise->all(array<std::shared_ptr<Promise<array<string>>>>{ redisCache->lrange(lookUpKey, 0, limit - 1) }); });
                swapsResultRaw = swapStrings->map([=](auto s) mutable
                {
                    return JSON->parse(s);
                }
                );
                auto formattedSwaps = swapsResultRaw->map([=](auto swap) mutable
                {
                    return (utils::assign(object{
                        , 
                        object::pair{std::string("directionText"), (swap["direction"] == 0) ? std::string("buy") : std::string("sell")}, 
                        object::pair{std::string("timestamp"), (swap["timestamp"]) ? any(((std::make_shared<Date>(swap["timestamp"])))->toISOString()) : any(nullptr)}
                    }, swap));
                }
                );
                responseData = object{
                    object::pair{std::string("swaps"), formattedSwaps}
                };
            }
            std::async([=]() { redisCache->setCompressed(cacheKey, responseData, 7); });
            return c["json"](responseData);
        }
        catch (const any& error)
        {
            logger["error"](std::string("Error in swaps history route:"), error);
            return c["json"](object{
                object::pair{std::string("swaps"), array<any>()}, 
                object::pair{std::string("error"), std::string("Failed to fetch swap history")}
            }, 500);
        }
    }
    );
}

MAIN
