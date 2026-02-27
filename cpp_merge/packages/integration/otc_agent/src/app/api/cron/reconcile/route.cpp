#include "route.hpp"
#include <string>

any GET(std::shared_ptr<NextRequest> request)
{
    auto authHeader = request->headers->get(std::string("authorization"));
    if (AND((process->env->NODE_ENV == std::string("production")), (!CRON_SECRET))) {
        console->error(std::string("[Reconciliation Cron] No CRON_SECRET configured in production"));
        return NextResponse->json(object{
            object::pair{std::string("error"), std::string("Server configuration error")}
        }, object{
            object::pair{std::string("status"), 500}
        });
    }
    if (AND((CRON_SECRET), (authHeader != std::string("Bearer ") + CRON_SECRET + string_empty))) {
        console->warn(std::string("[Reconciliation Cron] Unauthorized access attempt"), object{
            object::pair{std::string("ip"), OR((request->headers->get(std::string("x-forwarded-for"))), (request->headers->get(std::string("x-real-ip"))))}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        });
        return NextResponse->json(object{
            object::pair{std::string("error"), std::string("Unauthorized")}
        }, object{
            object::pair{std::string("status"), 401}
        });
    }
    console->log(std::string("[Reconciliation Cron] Starting reconciliation task..."));
    auto runtime = std::async([=]() { agentRuntime->getRuntime(); });
    auto maxRetries = 5;
    auto retries = 0;
    auto quoteService = runtime->getService(std::string("QuoteService"));
    while (AND((!quoteService), (retries < maxRetries)))
    {
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, 200);
        }
        ); });
        quoteService = runtime->getService(std::string("QuoteService"));
        retries++;
    }
    if (!quoteService) {
        console->error(std::string("[Reconciliation Cron] QuoteService not available after initialization"));
        return NextResponse->json(object{
            object::pair{std::string("error"), std::string("QuoteService not registered. Please check plugin configuration.")}
        }, object{
            object::pair{std::string("status"), 503}
        });
    }
    auto startTime = Date->now();
    std::async([=]() { runReconciliationTask(); });
    auto duration = Date->now() - startTime;
    console->log(std::string("[Reconciliation Cron] Completed in ") + duration + std::string("ms"));
    return NextResponse->json(object{
        object::pair{std::string("success"), true}, 
        object::pair{std::string("action"), std::string("reconcile_all")}, 
        object::pair{std::string("duration"), std::string("duration")}, 
        object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
    });
};


any POST(std::shared_ptr<NextRequest> request)
{
    return GET(request);
};


string CRON_SECRET = OR((process->env->CRON_SECRET), (process->env->RECONCILIATION_SECRET));

void Main(void)
{
}

MAIN
