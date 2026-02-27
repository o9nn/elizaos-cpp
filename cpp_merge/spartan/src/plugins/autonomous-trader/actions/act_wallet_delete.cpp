#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_wallet_delete.h"

std::shared_ptr<Action> userMetawalletList = object{
    object::pair{std:("name"), std:("USER_METAWALLET_LIST")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        console->log(std:("USER_METAWALLET_LIST validate"), message->metadata->authorId);
        if (!message->metadata->authorId) {
            console->log(std:("USER_METAWALLET_LIST validate - author not found"));
            return false;
        }
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        if (entityId == nullptr) return false;
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto reg = !!entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        if (!reg) return false;
        auto traderChainService = as<any>(runtime->getService(std:("TRADER_CHAIN")));
        if (!traderChainService) return false;
        auto traderStrategyService = as<any>(runtime->getService(std:("TRADER_STRATEGY")));
        if (!traderStrategyService) return false;
        return true;
    }
    }, 
    object::pair{std:("description"), std:("Allows a user to list all wallets they have")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("USER_METAWALLET_LIST handler"));
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        if (!email) {
            runtime->runtimeLogger->log(std:("Not registered"));
            return std::shared_ptr<Promise<boolean>>();
        }
        auto roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        auto traderStrategyService = as<any>(runtime->getService(std:("TRADER_STRATEGY")));
        auto stratgiesList = std::async([=]() { traderStrategyService["listActiveStrategies"](); });
        auto containsStrats = stratgiesList["filter"]([=](auto word) mutable
        {
            return message->content->text->includes(word);
        }
        );
        console->log(std:("containsStrats"), containsStrats);
        auto traderChainService = as<any>(runtime->getService(std:("TRADER_CHAIN")));
        auto chains = std::async([=]() { traderChainService["listActiveChains"](); });
        console->log(std:("chains"), chains);
        takeItPrivate(runtime, message, std:("List wallets: ") + JSON->stringify(email));
        responses->get_length() = 0;
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What wallets do I have")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here")}, 
            object::pair{std:("actions"), array<string>{ std:("USER_METAWALLET_LIST") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("list wallets")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Here")}, 
            object::pair{std:("actions"), array<string>{ std:("USER_METAWALLET_LIST") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want list all my wallets for you")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What?")}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
