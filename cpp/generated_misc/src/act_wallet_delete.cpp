#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_wallet_delete.h"

std::shared_ptr<Action> userMetawalletList = object{
    object::pair{std::string("name"), std::string("USER_METAWALLET_LIST")}, 
    object::pair{std::string("similes"), array<any>()}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        console->log(std::string("USER_METAWALLET_LIST validate"), message->metadata->authorId);
        if (!message->metadata->authorId) {
            console->log(std::string("USER_METAWALLET_LIST validate - author not found"));
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
        auto traderChainService = as<any>(runtime->getService(std::string("TRADER_CHAIN")));
        if (!traderChainService) return false;
        auto traderStrategyService = as<any>(runtime->getService(std::string("TRADER_STRATEGY")));
        if (!traderStrategyService) return false;
        return true;
    }
    }, 
    object::pair{std::string("description"), std::string("Allows a user to list all wallets they have")}, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std::string("USER_METAWALLET_LIST handler"));
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        if (!email) {
            runtime->runtimeLogger->log(std::string("Not registered"));
            return std::shared_ptr<Promise<boolean>>();
        }
        auto roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        auto traderStrategyService = as<any>(runtime->getService(std::string("TRADER_STRATEGY")));
        auto stratgiesList = std::async([=]() { traderStrategyService["listActiveStrategies"](); });
        auto containsStrats = stratgiesList["filter"]([=](auto word) mutable
        {
            return message->content->text->includes(word);
        }
        );
        console->log(std::string("containsStrats"), containsStrats);
        auto traderChainService = as<any>(runtime->getService(std::string("TRADER_CHAIN")));
        auto chains = std::async([=]() { traderChainService["listActiveChains"](); });
        console->log(std::string("chains"), chains);
        takeItPrivate(runtime, message, std::string("List wallets: ") + JSON->stringify(email));
        responses->get_length() = 0;
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What wallets do I have")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here")}, 
            object::pair{std::string("actions"), array<string>{ std::string("USER_METAWALLET_LIST") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("list wallets")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Here")}, 
            object::pair{std::string("actions"), array<string>{ std::string("USER_METAWALLET_LIST") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want list all my wallets for you")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What?")}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
