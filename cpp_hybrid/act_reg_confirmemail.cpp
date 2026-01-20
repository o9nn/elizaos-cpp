#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_reg_confirmemail.h"

any findGeneratedCode(any message, any length)
{
    auto pattern = std::make_shared<RegExp>(std::string("\b[A-Za-z0-9]{") + length + std::string("}\b"));
    auto match = message["match"](pattern);
    return (match) ? any(const_(match)[0]) : any(nullptr);
};


std::shared_ptr<Action> checkRegistrationCode = object{
    object::pair{std::string("name"), std::string("VERIFY_REGISTRATION_CODE")}, 
    object::pair{std::string("similes"), array<any>()}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        if (!message->metadata->authorId) return false;
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        auto containsGeneratedCode = findGeneratedCode(message->content->text, 16);
        if (containsGeneratedCode != nullptr) {
            runtime->runtimeLogger->log(std::string("VERIFY_REGISTRATION_CODE containsGeneratedCode"), type_of((containsGeneratedCode)), containsGeneratedCode);
        }
        return AND((AND((email), (containsGeneratedCode != nullptr))), (!email->data->verified));
    }
    }, 
    object::pair{std::string("description"), std::string("Allows a user set their email address")}, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std::string("VERIFY_REGISTRATION_CODE handler"));
        auto roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        console->log(std::string("VERIFY_REGISTRATION_CODE entity"), entity);
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        if (!email) {
            console->log(std::string("shouldnt be here"));
            return std::shared_ptr<Promise<boolean>>();
        }
        auto passedCode = findGeneratedCode(message->content->text, 16);
        if (passedCode == nullptr) {
            console->log(std::string("shouldnt be here"));
            return std::shared_ptr<Promise<boolean>>();
        }
        console->log(std::string("VERIFY_REGISTRATION_CODE email"), email, std::string("code"), passedCode);
        if (email->data->tries == undefined) email->data->tries = 0;
        if (email->data->tries > 3) {
            console->log(std::string("hacker..."));
            takeItPrivate(runtime, message, std::string("You can no longer validate, you must delete your registration and restart"));
            responses->get_length() = 0;
            return std::shared_ptr<Promise<boolean>>();
        }
        if (passedCode == email->data->code) {
            email->data->verified = true;
            takeItPrivate(runtime, message, std::string("Looks good, you are now registered and have access to my services"));
        } else {
            email->data->tries++;
            takeItPrivate(runtime, message, std::string("That does not match my records, please double check, it is case sensitive"));
        }
        responses->get_length() = 0;
        std::async([=]() { runtime->updateComponent(object{
            object::pair{std::string("id"), email->id}, 
            object::pair{std::string("worldId"), roomDetails->worldId}, 
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("sourceEntityId"), message->entityId}, 
            object::pair{std::string("entityId"), entityId}, 
            object::pair{std::string("type"), EMAIL_TYPE}, 
            object::pair{std::string("data"), email->data}, 
            object::pair{std::string("agentId"), runtime->agentId}
        }); });
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("This is my code you sent CODE")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll check it to see if it's correct")}, 
            object::pair{std::string("actions"), array<string>{ std::string("VERIFY_REGISTRATION_CODE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("This is the code openai sent CODE")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("actions"), array<string>{ std::string("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("what was the code you emailed me?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'm not going to tell you")}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
