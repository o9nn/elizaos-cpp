#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_wallet_create.h"

std::shared_ptr<Action> walletCreate = object{
    object::pair{std::string("name"), std::string("WALLET_CREATION")}, 
    object::pair{std::string("similes"), array<any>()}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        auto traderChainService = as<any>(runtime->getService(std::string("TRADER_STRATEGY")));
        return traderChainService;
    }
    }, 
    object::pair{std::string("description"), std::string("Allows a user to create a wallet")}, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std::string("WALLET_CREATION handler"));
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        if (!email) {
            runtime->runtimeLogger->log(std::string("Not registered"));
            messageReply(runtime, message, std::string("You need to sign up for my services first"), responses);
            responses->get_length() = 0;
            return std::shared_ptr<Promise<boolean>>();
        }
        auto traderChainService = as<any>(runtime->getService(std::string("TRADER_STRATEGY")));
        auto stratgiesList = std::async([=]() { traderChainService["listActiveStrategies"](); });
        console->log(std::string("stratgiesList"), stratgiesList);
        takeItPrivate(runtime, message, std::string("Hrm youve already signed up, here are the available strategies: \
-") + stratgiesList["join"](std::string("\
-")) + std::string("\
"));
        responses->get_length() = 0;
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to create a wallet for autonomous trading")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll help you get started")}, 
            object::pair{std::string("actions"), array<string>{ std::string("WALLET_CREATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to autotrade")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What strategy u wanna use")}, 
            object::pair{std::string("actions"), array<string>{ std::string("WALLET_CREATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'd like to trade")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("based. what strategy u want me to use")}, 
            object::pair{std::string("actions"), array<string>{ std::string("WALLET_CREATION") }}
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
            object::pair{std::string("text"), std::string("generate a wallet")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll help generate one, what trading strategy do you want to use?")}, 
            object::pair{std::string("actions"), array<string>{ std::string("WALLET_CREATION") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
