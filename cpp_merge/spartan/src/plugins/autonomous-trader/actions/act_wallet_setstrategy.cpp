#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_wallet_setstrategy.h"

std::shared_ptr<Action> setStrategy = object{
    object::pair{std:("name"), std:("WALLET_SETSTRAT")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        console->log(std:("WALLET_SETSTRAT validate"), message->metadata->authorId);
        if (!message->metadata->authorId) {
            console->log(std:("WALLET_SETSTRAT validate - author not found"));
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
        auto stratgiesList = std::async([=]() { traderStrategyService["listActiveStrategies"](); });
        auto containsStrat = stratgiesList["some"]([=](auto word) mutable
        {
            return message->content->text->includes(word);
        }
        );
        console->log(std:("containsStrat"), containsStrat, message->content->text);
        return containsStrat;
    }
    }, 
    object::pair{std:("description"), std:("Allows a user to create a wallet with a strategy")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("WALLET_SETSTRAT handler"));
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
        auto newData = email->data;
        if (newData->metawallets == undefined) newData->metawallets = array<any>();
        auto newWallet = object{
            object::pair{std:("strategy"), const_(containsStrats)[0]}
        };
        auto keypairs = object{};
        for (auto& c : chains)
        {
            console->log(std:("chain"), c);
            auto kp = std::async([=]() { traderChainService["makeKeypair"](c); });
            console->log(std:("kp"), kp);
            keypairs[c] = kp;
        }
        newWallet["keypairs"] = keypairs;
        console->log(std:("newWallet"), newWallet);
        takeItPrivate(runtime, message, std:("Made a meta-wallet ") + JSON->stringify(newWallet) + std:(" please fund it to start trading"));
        newData->metawallets->push(newWallet);
        newData->metawallets = array<object>{ newWallet };
        std::async([=]() { runtime->updateComponent(object{
            object::pair{std:("id"), email->id}, 
            object::pair{std:("worldId"), roomDetails->worldId}, 
            object::pair{std:("roomId"), message->roomId}, 
            object::pair{std:("sourceEntityId"), message->entityId}, 
            object::pair{std:("entityId"), entityId}, 
            object::pair{std:("type"), EMAIL_TYPE}, 
            object::pair{std:("data"), newData}, 
            object::pair{std:("agentId"), runtime->agentId}
        }); });
        responses->get_length() = 0;
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to create a wallet for autonomous trading using X trading strategy")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help you get started")}, 
            object::pair{std:("actions"), array<string>{ std:("WALLET_SETSTRAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to autotrade with X trading strategy")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Based")}, 
            object::pair{std:("actions"), array<string>{ std:("WALLET_SETSTRAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'd like to trade via X trading strategy")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Based")}, 
            object::pair{std:("actions"), array<string>{ std:("WALLET_SETSTRAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to trade with a friend")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("actions"), array<string>{ std:("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("generate a wallet using X trading strategy")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help generate one")}, 
            object::pair{std:("actions"), array<string>{ std:("WALLET_SETSTRAT") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
