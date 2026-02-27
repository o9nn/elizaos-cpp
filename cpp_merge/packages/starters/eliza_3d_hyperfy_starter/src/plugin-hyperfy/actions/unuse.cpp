#include "unuse.hpp"

std::shared_ptr<Action> hyperfyUnuseItemAction = object{
    object::pair{std:("name"), std:("HYPERFY_UNUSE_ITEM")}, 
    object::pair{std:("similes"), array<string>{ std:("RELEASE_ITEM"), std:("DROP_ITEM"), std:("CANCEL_INTERACTION") }}, 
    object::pair{std:("description"), std:("Drops or stops interacting with the currently held item; use when a player tells you to release it or you're done using it.")}, 
    object::pair{std:("validate"), [=](auto runtime) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        return AND((AND((!!service), (service->isConnected()))), (!!world->actions));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto _message, auto _state, auto _options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        auto actions = as<any>(world->actions);
        if (OR((OR((!service), (!world))), (!actions))) {
            logger->error(std:("[UNUSE ITEM] Hyperfy service, world, or actions system not found."));
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("Error: Cannot unuse item. Required systems are unavailable.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        logger->info(std:("[UNUSE ITEM] Attempting to release current action."));
        actions->releaseAction();
        std::async([=]() { callback(object{
            object::pair{std:("text"), std:("Item released.")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_UNUSE_ITEM") }}, 
            object::pair{std:("source"), std:("hyperfy")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("status"), std:("released")}
            }}
        }); });
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Drop it now.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Item released.")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_UNUSE_ITEM") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Stop using that.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Item released.")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_UNUSE_ITEM") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
