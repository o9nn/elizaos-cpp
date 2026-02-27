#include "character.hpp"
#include <string>

std::shared_ptr<CharacterValidationResult> validateCharacter(any data)
{
    auto result = characterSchema->safeParse(data);
    if (result->success) {
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("data"), as<std::shared_ptr<Character>>(result->data)}
        };
    }
    return object{
        object::pair{std::string("success"), false}, 
        object::pair{std::string("error"), object{
            object::pair{std::string("message"), std::string("Character validation failed: ") + result->error->message + string_empty}, 
            object::pair{std::string("issues"), result->error->issues}
        }}
    };
};


std::shared_ptr<CharacterValidationResult> parseAndValidateCharacter(string jsonString)
{
    try
    {
        auto parsed = JSON->parse(jsonString);
        return validateCharacter(parsed);
    }
    catch (const any& error)
    {
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("message"), std::string("Invalid JSON: ") + (is<Error>(error)) ? any(error->message) (std::string("Unknown JSON parsing error")) + string_empty}
            }}
        };
    }
};


any isValidCharacter(any data)
{
    return validateCharacter(data)->success;
};


any uuidSchema = z->string()->regex((new RegExp(std::string("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$"))), std::string("Invalid UUID format"));
any contentSchema = z->object(object{
    object::pair{std::string("text"), z->string()->optional()}, 
    object::pair{std::string("thought"), z->string()->optional()}, 
    object::pair{std::string("actions"), z->array(z->string())->optional()}, 
    object::pair{std::string("providers"), z->array(z->string())->optional()}, 
    object::pair{std::string("source"), z->string()->optional()}, 
    object::pair{std::string("target"), z->string()->optional()}, 
    object::pair{std::string("url"), z->string()->optional()}, 
    object::pair{std::string("inReplyTo"), uuidSchema->optional()}, 
    object::pair{std::string("attachments"), z->array(z->any())->optional()}, 
    object::pair{std::string("channelType"), z->string()->optional()}
})->passthrough();
any messageExampleSchema = z->object(object{
    object::pair{std::string("name"), z->string()}, 
    object::pair{std::string("content"), contentSchema}
});
any directoryItemSchema = z->object(object{
    object::pair{std::string("directory"), z->string()}, 
    object::pair{std::string("shared"), z->boolean()->optional()}
});
any knowledgeItemSchema = z->union(array<any>{ z->string(), z->object(object{
    object::pair{std::string("path"), z->string()}, 
    object::pair{std::string("shared"), z->boolean()->optional()}
}), directoryItemSchema });
any templateTypeSchema = z->union(array<any>{ z->string(), z->function()->optional() });
any styleSchema = z->object(object{
    object::pair{std::string("all"), z->array(z->string())->optional()}, 
    object::pair{std::string("chat"), z->array(z->string())->optional()}, 
    object::pair{std::string("post"), z->array(z->string())->optional()}
})->optional();
any settingsSchema = z->record(z->union(array<any>{ z->string(), z->boolean(), z->number(), z->any() }))->optional();
any secretsSchema = z->record(z->union(array<any>{ z->string(), z->boolean(), z->number() }))->optional();
any characterSchema = z->object(object{
    object::pair{std::string("id"), uuidSchema->optional()}, 
    object::pair{std::string("name"), z->string()->min(1, std::string("Character name is required"))}, 
    object::pair{std::string("username"), z->string()->optional()}, 
    object::pair{std::string("system"), z->string()->optional()}, 
    object::pair{std::string("templates"), z->record(templateTypeSchema)->optional()}, 
    object::pair{std::string("bio"), z->union(array<any>{ z->string(), z->array(z->string()) })}, 
    object::pair{std::string("messageExamples"), z->array(z->array(messageExampleSchema))->optional()}, 
    object::pair{std::string("postExamples"), z->array(z->string())->optional()}, 
    object::pair{std::string("topics"), z->array(z->string())->optional()}, 
    object::pair{std::string("adjectives"), z->array(z->string())->optional()}, 
    object::pair{std::string("knowledge"), z->array(knowledgeItemSchema)->optional()}, 
    object::pair{std::string("plugins"), z->array(z->string())->optional()}, 
    object::pair{std::string("settings"), settingsSchema}, 
    object::pair{std::string("secrets"), secretsSchema}, 
    object::pair{std::string("style"), styleSchema}
})->strict();

void Main(void)
{
}

MAIN
