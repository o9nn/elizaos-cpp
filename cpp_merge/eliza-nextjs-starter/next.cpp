#include "next.config.h"
#include <string>

std::shared_ptr<NextConfig> nextConfig = object{
    object::pair{std::string("compiler"), object{
        object::pair{std::string("removeConsole"), process->env->NODE_ENV == std::string("production")}
    }}, 
    object::pair{std::string("experimental"), object{
        object::pair{std::string("inlineCss"), true}
    }}, 
    object::pair{std::string("webpack"), [=](auto config) mutable
    {
        config["plugins"]["push"](std::make_shared<webpack->IgnorePlugin>(object{
            object::pair{std::string("resourceRegExp"), (new RegExp(std::string("^pg-native$|^cloudflare:sockets")))}
        }), std::make_shared<webpack->IgnorePlugin>(object{
        }));
        return utils::assign(object{
            , 
            object::pair{std::string("resolve"), utils::assign(object{
                , 
                object::pair{std::string("fallback"), utils::assign(object{
                    , 
                    object::pair{std::string("fs"), false}, 
                    object::pair{std::string("net"), false}, 
                    object::pair{std::string("tls"), false}, 
                    object::pair{std::string("async_hooks"), false}, 
                    object::pair{std::string("worker_threads"), false}
                }, config["resolve"]["fallback"])}
            }, config["resolve"])}
        }, config);
    }
    }, 
    , 
    , 
    object::pair{std::string("skipTrailingSlashRedirect"), true}
};

void Main(void)
{
}

MAIN
