#include "act_wallet_create.hpp"

std::shared_ptr<Action> walletCreate = object{
    object::pair{std:("name"), std:("WALLET_CREATION")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        auto traderChainService = as<any>(runtime->getService(std:("TRADER_STRATEGY")));
        return traderChainService;
    }
    }, 
    object::pair{std:("description"), std:("Allows a user to create a wallet")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("WALLET_CREATION handler"));
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        if (!email) {
            runtime->runtimeLogger->log(std:("Not registered"));
            messageReply(runtime, message, std:("You need to sign up for my services first"), responses);
            responses->get_length() = 0;
            return std::shared_ptr<Promise<boolean>>();
        }
        auto traderChainService = as<any>(runtime->getService(std:("TRADER_STRATEGY")));
        auto stratgiesList = std::async([=]() { traderChainService["listActiveStrategies"](); });
        console->log(std:("stratgiesList"), stratgiesList);
        takeItPrivate(runtime, message, std:("Hrm youve already signed up, here are the available strategies: \
-") + stratgiesList["join"](std:("\
-")) + std:("\
"));
        responses->get_length() = 0;
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to create a wallet for autonomous trading")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help you get started")}, 
            object::pair{std:("actions"), array<string>{ std:("WALLET_CREATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to autotrade")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What strategy u wanna use")}, 
            object::pair{std:("actions"), array<string>{ std:("WALLET_CREATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'd like to trade")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("based. what strategy u want me to use")}, 
            object::pair{std:("actions"), array<string>{ std:("WALLET_CREATION") }}
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
            object::pair{std:("text"), std:("generate a wallet")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help generate one, what trading strategy do you want to use?")}, 
            object::pair{std:("actions"), array<string>{ std:("WALLET_CREATION") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
