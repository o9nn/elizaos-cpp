#include "route.hpp"

any GET()
{
    try
    {
        auto now = Date->now();
        if (AND((cachedData), ((now - cacheTimestamp) < CACHE_DURATION))) {
            console->log(std:("Returning cached registry data"));
            return NextResponse->json(cachedData, object{
                object::pair{std:("headers"), object{
                    object::pair{std:("Cache-Control"), std:("public, s-maxage=1800, stale-while-revalidate=3600")}, 
                    object::pair{std:("Content-Type"), std:("application/json")}
                }}
            });
        }
        auto githubToken = OR((process->env->GITHUB_TOKEN), (process->env->GH_TOKEN));
        if (!githubToken) {
            return NextResponse->json(object{
                object::pair{std:("error"), std:("GitHub token not configured on server")}
            }, object{
                object::pair{std:("status"), 500}
            });
        }
        console->log(std:("Parsing registry data..."));
        auto timeoutPromise = std::make_shared<Promise>([=](auto _, auto reject) mutable
        {
            return setTimeout([=]() mutable
            {
                return reject(std::make_shared<Error>(std:("Registry parsing timeout")));
            }
            , 25000);
        }
        );
        auto parsePromise = parseRegistry(githubToken);
        auto result = as<std::shared_ptr<CachedRegistry>>(std::async([=]() { Promise->race(array<any>{ parsePromise, timeoutPromise }); }));
        cachedData = result;
        cacheTimestamp = now;
        console->log(std:("Registry parsing completed successfully"));
        return NextResponse->json(result, object{
            object::pair{std:("headers"), object{
                object::pair{std:("Cache-Control"), std:("public, s-maxage=1800, stale-while-revalidate=3600")}, 
                object::pair{std:("Content-Type"), std:("application/json")}
            }}
        });
    }
    catch (const any& error)
    {
        auto errorMessage = (is<Error>(error)) ? any(error->message) (std:("Unknown error"));
        console->error(std:("Registry parsing failed:"), errorMessage);
        if (cachedData) {
            console->log(std:("Returning stale cached data due to error"));
            return NextResponse->json(utils::assign(object{
                , 
                object::pair{std:("warning"), std:("Data may be stale due to parsing error")}, 
                object::pair{std:("error"), errorMessage}
            }, cachedData), object{
                object::pair{std:("headers"), object{
                    object::pair{std:("Cache-Control"), std:("public, s-maxage=300, stale-while-revalidate=3600")}, 
                    object::pair{std:("Content-Type"), std:("application/json")}
                }}
            });
        }
        return NextResponse->json(object{
            object::pair{std:("error"), std:("Failed to parse registry")}, 
            object::pair{std:("message"), errorMessage}, 
            object::pair{std:("lastUpdatedAt"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std:("registry"), object{}}
        }, object{
            object::pair{std:("status"), 500}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}
        });
    }
};


any OPTIONS()
{
    return NextResponse->json(object{}, object{
        object::pair{std:("headers"), object{
            object::pair{std:("Access-Control-Allow-Origin"), std:("*")}, 
            object::pair{std:("Access-Control-Allow-Methods"), std:("GET, OPTIONS")}, 
            object::pair{std:("Access-Control-Allow-Headers"), std:("Content-Type")}
        }}
    });
};


any cachedData = nullptr;
double cacheTimestamp = 0;
double CACHE_DURATION = 30 * 60 * 1000;

void Main(void)
{
}

MAIN
