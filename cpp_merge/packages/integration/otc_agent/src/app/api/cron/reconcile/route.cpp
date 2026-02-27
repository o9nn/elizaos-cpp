#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/app/api/cron/reconcile/route.h"

any GET(std::shared_ptr<NextRequest> request)
{
    auto authHeader = request->headers->get(std:("authorization"));
    if (AND((process->env->NODE_ENV == std:("production")), (!CRON_SECRET))) {
        console->error(std:("[Reconciliation Cron] No CRON_SECRET configured in production"));
        return NextResponse->json(object{
            object::pair{std:("error"), std:("Server configuration error")}
        }, object{
            object::pair{std:("status"), 500}
        });
    }
    if (AND((CRON_SECRET), (authHeader != std:("Bearer ") + CRON_SECRET + string_empty))) {
        console->warn(std:("[Reconciliation Cron] Unauthorized access attempt"), object{
            object::pair{std:("ip"), OR((request->headers->get(std:("x-forwarded-for"))), (request->headers->get(std:("x-real-ip"))))}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        });
        return NextResponse->json(object{
            object::pair{std:("error"), std:("Unauthorized")}
        }, object{
            object::pair{std:("status"), 401}
        });
    }
    console->log(std:("[Reconciliation Cron] Starting reconciliation task..."));
    auto runtime = std::async([=]() { agentRuntime->getRuntime(); });
    auto maxRetries = 5;
    auto retries = 0;
    auto quoteService = runtime->getService(std:("QuoteService"));
    while (AND((!quoteService), (retries < maxRetries)))
    {
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, 200);
        }
        ); });
        quoteService = runtime->getService(std:("QuoteService"));
        retries++;
    }
    if (!quoteService) {
        console->error(std:("[Reconciliation Cron] QuoteService not available after initialization"));
        return NextResponse->json(object{
            object::pair{std:("error"), std:("QuoteService not registered. Please check plugin configuration.")}
        }, object{
            object::pair{std:("status"), 503}
        });
    }
    auto startTime = Date->now();
    std::async([=]() { runReconciliationTask(); });
    auto duration = Date->now() - startTime;
    console->log(std:("[Reconciliation Cron] Completed in ") + duration + std:("ms"));
    return NextResponse->json(object{
        object::pair{std:("success"), true}, 
        object::pair{std:("action"), std:("reconcile_all")}, 
        object::pair{std:("duration"), std:("duration")}, 
        object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
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
