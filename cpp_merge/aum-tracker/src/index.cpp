#include "/home/runner/work/elizaos-cpp/elizaos-cpp/aum-tracker/src/index.h"

void startupPrefetch()
{
    console->log(std::string("🚀 Starting full prefetch on server startup..."));
    try
    {
        auto prefetchService = std::make_shared<PrefetchService>(object{
            object::pair{std::string("forceRefresh"), true}, 
            object::pair{std::string("resumeFromFailures"), true}
        });
        prefetchService->run()->_catch([=](auto error) mutable
        {
            console->error(std::string("❌ Startup prefetch failed:"), error);
        }
        );
    }
    catch (const any& error)
    {
        console->error(std::string("❌ Failed to start auto-prefetch:"), error);
    }
};


any app = std::make_shared<Hono>();

void Main(void)
{
    app->use(std::string("*"), logger());
    app->use(std::string("*"), cors());
    app->use(jsxRenderer());
    app->use(std::string("/static/*"), serveStatic(object{
        object::pair{std::string("root"), std::string("./src/public")}
    }));
    app->route(std::string("/api"), api);
    app->get(std::string("/"), [=](auto c) mutable
    {
        return c["html"](ModernLayout(object{
            object::pair{std::string("title"), std::string("Spartan AUM")}, 
            object::pair{std::string("children"), ModernDashboard(object{})}
        }));
    }
    );
    app->get(std::string("/wallet/:address"), [=](auto c) mutable
    {
        auto address = c["req"]["param"](std::string("address"));
        return c["html"](ModernLayout(object{
            object::pair{std::string("title"), std::string("Wallet ") + address["substring"](0, 6) + std::string("...") + address["slice"](-4) + string_empty}, 
            object::pair{std::string("children"), WalletDetail(object{
                object::pair{std::string("address"), std::string("address")}
            })}
        }));
    }
    );
    app->get(std::string("/token/:mint"), [=](auto c) mutable
    {
        auto mint = c["req"]["param"](std::string("mint"));
        return c["html"](ModernLayout(object{
            object::pair{std::string("title"), std::string("Token Details")}, 
            object::pair{std::string("children"), TokenDetail(object{
                object::pair{std::string("mint"), std::string("mint")}
            })}
        }));
    }
    );
    app->get(std::string("/health"), [=](auto c) mutable
    {
        return c["json"](object{
            object::pair{std::string("status"), std::string("healthy")}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std::string("version"), std::string("1.0.0")}
        });
    }
    );
    app->notFound([=](auto c) mutable
    {
        return c["html"](ModernLayout(object{
            object::pair{std::string("title"), std::string("Page Not Found")}, 
            object::pair{std::string("children"), std::string("\
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
        console->error(std::string("Server error:"), err);
        return c["html"](ModernLayout(object{
            object::pair{std::string("title"), std::string("Server Error")}, 
            object::pair{std::string("children"), std::string("\
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
