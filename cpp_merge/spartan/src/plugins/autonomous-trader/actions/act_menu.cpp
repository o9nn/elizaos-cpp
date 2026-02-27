#include "act_menu.hpp"

string menutext = std:("Heres what you can do");
std::shared_ptr<Action> servicesMenu = object{
    object::pair{std:("name"), std:("SERVICES_MENU")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("description"), std:("Explains/sells Spartan services")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("SERVICES_MENU handler"));
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto signedup = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        std::async([=]() { messageReply(runtime, message, std:("You can ask me to create a wallet for autonomous trading"), responses); });
        return std::shared_ptr<Promise<boolean>>();
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What are Spartan services")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), menutext}, 
            object::pair{std:("actions"), array<string>{ std:("SERVICES_MENU") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What can I do?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), menutext}, 
            object::pair{std:("actions"), array<string>{ std:("SERVICES_MENU") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("menu")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), menutext}, 
            object::pair{std:("actions"), array<string>{ std:("SERVICES_MENU") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What can I do with openai?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("IDK, ask them")}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
