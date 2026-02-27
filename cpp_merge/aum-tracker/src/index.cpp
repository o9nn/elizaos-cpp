#include "/home/runner/work/elizaos-cpp/elizaos-cpp/aum-tracker/src/index.h"

void startupPrefetch()
{
    console->log(std:("🚀 Starting full prefetch on server startup..."));
    try
    {
        auto prefetchService = std::make_shared<PrefetchService>(object{
            object::pair{std:("forceRefresh"), true}, 
            object::pair{std:("resumeFromFailures"), true}
        });
        prefetchService->run()->_catch([=](auto error) mutable
        {
            console->error(std:("❌ Startup prefetch failed:"), error);
        }
        );
    }
    catch (const any& error)
    {
        console->error(std:("❌ Failed to start auto-prefetch:"), error);
    }
};


any app = std::make_shared<Hono>();

void Main(void)
{
    app->use(std:("*"), logger());
    app->use(std:("*"), cors());
    app->use(jsxRenderer());
    app->use(std:("/static/*"), serveStatic(object{
        object::pair{std:("root"), std:("./src/public")}
    }));
    app->route(std:("/api"), api);
    app->get(std:("/"), [=](auto c) mutable
    {
        return c["html"](ModernLayout(object{
            object::pair{std:("title"), std:("Spartan AUM")}, 
            object::pair{std:("children"), ModernDashboard(object{})}
        }));
    }
    );
    app->get(std:("/wallet/:address"), [=](auto c) mutable
    {
        auto address = c["req"]["param"](std:("address"));
        return c["html"](ModernLayout(object{
            object::pair{std:("title"), std:("Wallet ") + address["substring"](0, 6) + std:("...") + address["slice"](-4) + string_empty}, 
            object::pair{std:("children"), WalletDetail(object{
                object::pair{std:("address"), std:("address")}
            })}
        }));
    }
    );
    app->get(std:("/token/:mint"), [=](auto c) mutable
    {
        auto mint = c["req"]["param"](std:("mint"));
        return c["html"](ModernLayout(object{
            object::pair{std:("title"), std:("Token Details")}, 
            object::pair{std:("children"), TokenDetail(object{
                object::pair{std:("mint"), std:("mint")}
            })}
        }));
    }
    );
    app->get(std:("/health"), [=](auto c) mutable
    {
        return c["json"](object{
            object::pair{std:("status"), std:("healthy")}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std:("version"), std:("1.0.0")}
        });
    }
    );
    app->notFound([=](auto c) mutable
    {
        return c["html"](ModernLayout(object{
            object::pair{std:("title"), std:("Page Not Found")}, 
            object::pair{std:("children"), std:("\
        <div class="glass-card rounded-2xl p-8 text-center">\
          <h1 class="text-2xl font-bold text-white mb-4">404 - Page Not Found</h1>\
          <p class="text-gray-400 mb-4">The page you're looking for doesn't exist.</p>\
          <a href="/" class="px-4 py-2 bg-indigo-600 text-white rounded-lg hover:bg-indigo-700 transition-colors">\
            Go Home\
          </a>\
        </div>\
      ")}
        }));
    }
    );
    app->onError([=](auto err, auto c) mutable
    {
        console->error(std:("Server error:"), err);
        return c["html"](ModernLayout(object{
            object::pair{std:("title"), std:("Server Error")}, 
            object::pair{std:("children"), std:("\
        <div class="glass-card rounded-2xl p-8 text-center">\
          <h1 class="text-2xl font-bold text-white mb-4">500 - Server Error</h1>\
          <p class="text-gray-400 mb-4">Something went wrong on our end.</p>\
          <a href="/" class="px-4 py-2 bg-indigo-600 text-white rounded-lg hover:bg-indigo-700 transition-colors">\
            Go Home\
          </a>\
        </div>\
      ")}
        }));
    }
    );
    setTimeout(startupPrefetch, 5000);
}

MAIN
