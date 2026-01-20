#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/actions/unuse.h"

std::shared_ptr<Action> hyperfyUnuseItemAction = object{
    object::pair{std::string("name"), std::string("HYPERFY_UNUSE_ITEM")}, 
    object::pair{std::string("similes"), array<string>{ std::string("RELEASE_ITEM"), std::string("DROP_ITEM"), std::string("CANCEL_INTERACTION") }}, 
    object::pair{std::string("description"), std::string("Drops or stops interacting with the currently held item; use when a player tells you to release it or you're done using it.")}, 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        return AND((AND((!!service), (service->isConnected()))), (!!world->actions));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto _message, auto _state, auto _options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        auto actions = as<any>(world->actions);
        if (OR((OR((!service), (!world))), (!actions))) {
            logger->error(std::string("[UNUSE ITEM] Hyperfy service, world, or actions system not found."));
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("Error: Cannot unuse item. Required systems are unavailable.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        logger->info(std::string("[UNUSE ITEM] Attempting to release current action."));
        actions->releaseAction();
        std::async([=]() { callback(object{
            object::pair{std::string("text"), std::string("Item released.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_UNUSE_ITEM") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("status"), std::string("released")}
            }}
        }); });
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Drop it now.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Item released.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_UNUSE_ITEM") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Stop using that.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Item released.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_UNUSE_ITEM") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
