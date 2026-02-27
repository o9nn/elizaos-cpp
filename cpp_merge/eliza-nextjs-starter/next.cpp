#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-nextjs-starter/next.config.h"

std::shared_ptr<NextConfig> nextConfig = object{
    object::pair{std:("compiler"), object{
        object::pair{std:("removeConsole"), process->env->NODE_ENV == std:("production")}
    }}, 
    object::pair{std:("experimental"), object{
        object::pair{std:("inlineCss"), true}
    }}, 
    object::pair{std:("webpack"), [=](auto config) mutable
    {
        config["plugins"]["push"](std::make_shared<webpack->IgnorePlugin>(object{
            object::pair{std:("resourceRegExp"), (new RegExp(std:("^pg-native$|^cloudflare:sockets")))}
        }), std::make_shared<webpack->IgnorePlugin>(object{
        }));
        return utils::assign(object{
            , 
            object::pair{std:("resolve"), utils::assign(object{
                , 
                object::pair{std:("fallback"), utils::assign(object{
                    , 
                    object::pair{std:("fs"), false}, 
                    object::pair{std:("net"), false}, 
                    object::pair{std:("tls"), false}, 
                    object::pair{std:("async_hooks"), false}, 
                    object::pair{std:("worker_threads"), false}
                }, config["resolve"]["fallback"])}
            }, config["resolve"])}
        }, config);
    }
    }, 
    , 
    , 
    object::pair{std:("skipTrailingSlashRedirect"), true}
};

void Main(void)
{
}

MAIN
