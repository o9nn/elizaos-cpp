#include "/home/runner/work/elizaos-cpp/elizaos-cpp/vercel-api/app/api/plugins/registry/route.h"

any GET()
{
    try
    {
        auto now = Date->now();
        if (AND((cachedData), ((now - cacheTimestamp) < CACHE_DURATION))) {
            console->log(std::string("Returning cached registry data"));
            return NextResponse->json(cachedData, object{
                object::pair{std::string("headers"), object{
                    object::pair{std::string("Cache-Control"), std::string("public, s-maxage=1800, stale-while-revalidate=3600")}, 
                    object::pair{std::string("Content-Type"), std::string("application/json")}
                }}
            });
        }
        auto githubToken = OR((process->env->GITHUB_TOKEN), (process->env->GH_TOKEN));
        if (!githubToken) {
            return NextResponse->json(object{
                object::pair{std::string("error"), std::string("GitHub token not configured on server")}
            }, object{
                object::pair{std::string("status"), 500}
            });
        }
        console->log(std::string("Parsing registry data..."));
        auto timeoutPromise = std::make_shared<Promise>([=](auto _, auto reject) mutable
        {
            return setTimeout([=]() mutable
            {
                return reject(std::make_shared<Error>(std::string("Registry parsing timeout")));
            }
            , 25000);
        }
        );
        auto parsePromise = parseRegistry(githubToken);
        auto result = as<std::shared_ptr<CachedRegistry>>(std::async([=]() { Promise->race(array<any>{ parsePromise, timeoutPromise }); }));
        cachedData = result;
        cacheTimestamp = now;
        console->log(std::string("Registry parsing completed successfully"));
        return NextResponse->json(result, object{
            object::pair{std::string("headers"), object{
                object::pair{std::string("Cache-Control"), std::string("public, s-maxage=1800, stale-while-revalidate=3600")}, 
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}
        });
    }
    catch (const any& error)
    {
        auto errorMessage = (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"));
        console->error(std::string("Registry parsing failed:"), errorMessage);
        if (cachedData) {
            console->log(std::string("Returning stale cached data due to error"));
            return NextResponse->json(utils::assign(object{
                , 
                object::pair{std::string("warning"), std::string("Data may be stale due to parsing error")}, 
                object::pair{std::string("error"), errorMessage}
            }, cachedData), object{
                object::pair{std::string("headers"), object{
                    object::pair{std::string("Cache-Control"), std::string("public, s-maxage=300, stale-while-revalidate=3600")}, 
                    object::pair{std::string("Content-Type"), std::string("application/json")}
                }}
            });
        }
        return NextResponse->json(object{
            object::pair{std::string("error"), std::string("Failed to parse registry")}, 
            object::pair{std::string("message"), errorMessage}, 
            object::pair{std::string("lastUpdatedAt"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std::string("registry"), object{}}
        }, object{
            object::pair{std::string("status"), 500}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}
        });
    }
};


any OPTIONS()
{
    return NextResponse->json(object{}, object{
        object::pair{std::string("headers"), object{
            object::pair{std::string("Access-Control-Allow-Origin"), std::string("*")}, 
            object::pair{std::string("Access-Control-Allow-Methods"), std::string("GET, OPTIONS")}, 
            object::pair{std::string("Access-Control-Allow-Headers"), std::string("Content-Type")}
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
