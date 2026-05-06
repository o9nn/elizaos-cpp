#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_wallet_setstrategy.h"

std::shared_ptr<Action> setStrategy = object{
    object::pair{std::string("name"), std::string("WALLET_SETSTRAT")}, 
    object::pair{std::string("similes"), array<any>()}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        console->log(std::string("WALLET_SETSTRAT validate"), message->metadata->authorId);
        if (!message->metadata->authorId) {
            console->log(std::string("WALLET_SETSTRAT validate - author not found"));
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
        auto stratgiesList = std::async([=]() { traderStrategyService["listActiveStrategies"](); });
        auto containsStrat = stratgiesList["some"]([=](auto word) mutable
        {
            return message->content->text->includes(word);
        }
        );
        console->log(std::string("containsStrat"), containsStrat, message->content->text);
        return containsStrat;
    }
    }, 
    object::pair{std::string("description"), std::string("Allows a user to create a wallet with a strategy")}, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std::string("WALLET_SETSTRAT handler"));
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
        auto newData = email->data;
        if (newData->metawallets == undefined) newData->metawallets = array<any>();
        auto newWallet = object{
            object::pair{std::string("strategy"), const_(containsStrats)[0]}
        };
        auto keypairs = object{};
        for (auto& c : chains)
        {
            console->log(std::string("chain"), c);
            auto kp = std::async([=]() { traderChainService["makeKeypair"](c); });
            console->log(std::string("kp"), kp);
            keypairs[c] = kp;
        }
        newWallet["keypairs"] = keypairs;
        console->log(std::string("newWallet"), newWallet);
        takeItPrivate(runtime, message, std::string("Made a meta-wallet ") + JSON->stringify(newWallet) + std::string(" please fund it to start trading"));
        newData->metawallets->push(newWallet);
        newData->metawallets = array<object>{ newWallet };
        std::async([=]() { runtime->updateComponent(object{
            object::pair{std::string("id"), email->id}, 
            object::pair{std::string("worldId"), roomDetails->worldId}, 
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("sourceEntityId"), message->entityId}, 
            object::pair{std::string("entityId"), entityId}, 
            object::pair{std::string("type"), EMAIL_TYPE}, 
            object::pair{std::string("data"), newData}, 
            object::pair{std::string("agentId"), runtime->agentId}
        }); });
        responses->get_length() = 0;
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to create a wallet for autonomous trading using X trading strategy")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll help you get started")}, 
            object::pair{std::string("actions"), array<string>{ std::string("WALLET_SETSTRAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to autotrade with X trading strategy")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Based")}, 
            object::pair{std::string("actions"), array<string>{ std::string("WALLET_SETSTRAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'd like to trade via X trading strategy")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Based")}, 
            object::pair{std::string("actions"), array<string>{ std::string("WALLET_SETSTRAT") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to trade with a friend")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("actions"), array<string>{ std::string("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("generate a wallet using X trading strategy")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll help generate one")}, 
            object::pair{std::string("actions"), array<string>{ std::string("WALLET_SETSTRAT") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
