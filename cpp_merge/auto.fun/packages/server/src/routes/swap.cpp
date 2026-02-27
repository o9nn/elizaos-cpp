#include "swap.hpp"

any codex = std::make_shared<Codex>(process->env->CODEX_API_KEY);
any router = std::make_shared<Hono<object>>();
any ChartParamsSchema = z->object(object{
    object::pair{std:("pairIndex"), z->string()->transform([=](auto val) mutable
    {
        return parseInt(val);
    }
    )}, 
    object::pair{std:("start"), z->string()->transform([=](auto val) mutable
    {
        return parseInt(val);
    }
    )}, 
    object::pair{std:("end"), z->string()->transform([=](auto val) mutable
    {
        return parseInt(val);
    }
    )}, 
    object::pair{std:("range"), z->string()->transform([=](auto val) mutable
    {
        return parseInt(val);
    }
    )}, 
    object::pair{std:("token"), z->string()->min(32)->max(44)}
});

void Main(void)
{
    if (!process->env->CODEX_API_KEY) {
        logger["error"](std:("Missing CODEX_API_KEY from .env"));
        process->exit(1);
    }
    router->get(std:("/chart/:pairIndex/:start/:end/:range/:token"), honoCacheMiddleware(object{
        object::pair{std:("cacheName"), std:("chart-cache")}, 
        object::pair{std:("cacheControl"), std:("max-age=120")}, 
        object::pair{std:("wait"), true}
    }), [=](auto c) mutable
    {
        try
        {
            auto params = ChartParamsSchema->parse(c["req"]["param"]());
            auto redisCache = std::async([=]() { getGlobalRedisCache(); });
            auto from = moment()->utc()->subtract(30, std:("minutes"))->startOf(std:("minute"))->unix();
            auto to = moment()->utc()->endOf(std:("minute"))->unix();
            auto cacheKey = string_empty + from + std:(":") + to + std:(":1:") + params->token + string_empty;
            auto cache = std::async([=]() { redisCache->getCompressed(cacheKey); });
            if (cache) {
                return c["json"](cache);
            }
            auto data = std::async([=]() { fetchPriceChartData(from, to, 1, params->token); });
            std::async([=]() { redisCache->setCompressed(cacheKey, JSON->stringify(object{
                object::pair{std:("table"), data}
            }), 8); });
            return c["json"](object{
                object::pair{std:("table"), data}
            });
        }
        catch (const any& error)
        {
            if (is<z->ZodError>(error)) {
                c["json"](object{
                    object::pair{std:("error"), error["errors"]}
                }, 400);
            } else {
                logger["error"](error);
                c["json"](object{
                    object::pair{std:("error"), std:("Internal server error")}
                }, 500);
            }
        }
    }
    );
    router->post(std:("/creator-tokens"), [=](auto c) mutable
    {
        auto user = c["get"](std:("user"));
        if (!user) {
            return c["json"](object{
                object::pair{std:("message"), std:("Unauthorized")}
            }, 401);
        }
        auto db = getDB();
        auto tokensCreated = std::async([=]() { db["select"]()["from"](tokens)["where"](and(eq(tokens->creator, user["publicKey"]), eq(tokens->imported, 0)))["orderBy"](desc(tokens->createdAt)); });
        return c["json"](object{
            object::pair{std:("tokens"), tokensCreated}
        });
    }
    );
    router->get(std:("/swaps/:mint"), [=](auto c) mutable
    {
        try
        {
            auto mint = c["req"]["param"](std:("mint"));
            if (OR((OR((!mint), (mint["length"] < 32))), (mint["length"] > 44))) {
                return c["json"](object{
                    object::pair{std:("error"), std:("Invalid mint address")}
                }, 400);
            }
            auto limit = 50;
            auto redisCache = std::async([=]() { getGlobalRedisCache(); });
            auto cacheKey = std:("swapsListCompressed:") + mint + string_empty;
            auto lookUpKey = std:("swapsList:") + mint + string_empty;
            auto cache = std::async([=]() { redisCache->getCompressed(cacheKey); });
            if (cache) {
                return c["json"](cache);
            }
            auto responseData = object{};
            auto db = getDB();
            auto token = const_((std::async([=]() { db["select"](object{
                object::pair{std:("imported"), tokens->imported}, 
                object::pair{std:("status"), tokens->status}
            })["from"](tokens)["where"](eq(tokens->mint, mint))["limit"](1); })))[0];
            if (!token) throw any(std::make_shared<Error>(std:("Token not found")));
            auto isCodex = useCodex(token);
            if (isCodex) {
                shared data = std::async([=]() { codex->queries->getTokenEvents(object{
                    object::pair{std:("query"), object{
                        object::pair{std:("address"), mint}, 
                        object::pair{std:("networkId"), 1399811149}, 
                        object::pair{std:("eventType"), EventType->Swap}
                    }}, 
                    object::pair{std:("direction"), RankingDirection->Desc}, 
                    object::pair{std:("limit"), std:("limit")}
                }); });
                auto items = data->getTokenEvents->items;
                responseData = object{
                    object::pair{std:("swaps"), items->map([=](auto swap) mutable
                    {
                        return object{
                            object::pair{std:("account"), OR((swap["maker"]), (std:("NA")))}, 
                            object::pair{std:("swapType"), OR((swap["eventDisplayType"]), (std:("Buy")))}, 
                            object::pair{std:("solana"), OR((swap["data"]["priceBaseTokenTotal"]), (std:("0")))}, 
                            object::pair{std:("tokenAmount"), OR((swap["data"]["amountNonLiquidityToken"]), (std:("0")))}, 
                            object::pair{std:("transactionHash"), OR((swap["transactionHash"]), (string_empty))}, 
                            object::pair{std:("timestamp"), OR((swap["timestamp"] * 1000), (0))}, 
                            object::pair{std:("usdValue"), OR((swap["data"]["priceUsdTotal"]), (nullptr))}
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
                        object::pair{std:("directionText"), (swap["direction"] == 0) ? std:("buy") : std:("sell")}, 
                        object::pair{std:("timestamp"), (swap["timestamp"]) ? any(((std::make_shared<Date>(swap["timestamp"])))->toISOString()) (nullptr)}
                    }, swap));
                }
                );
                responseData = object{
                    object::pair{std:("swaps"), formattedSwaps}
                };
            }
            std::async([=]() { redisCache->setCompressed(cacheKey, responseData, 7); });
            return c["json"](responseData);
        }
        catch (const any& error)
        {
            logger["error"](std:("Error in swaps history route:"), error);
            return c["json"](object{
                object::pair{std:("swaps"), array<any>()}, 
                object::pair{std:("error"), std:("Failed to fetch swap history")}
            }, 500);
        }
    }
    );
}

MAIN
