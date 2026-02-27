#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/schemas/character.h"

std::shared_ptr<CharacterValidationResult> validateCharacter(any data)
{
    auto result = characterSchema->safeParse(data);
    if (result->success) {
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("data"), as<std::shared_ptr<Character>>(result->data)}
        };
    }
    return object{
        object::pair{std:("success"), false}, 
        object::pair{std:("error"), object{
            object::pair{std:("message"), std:("Character validation failed: ") + result->error->message + string_empty}, 
            object::pair{std:("issues"), result->error->issues}
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
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("message"), std:("Invalid JSON: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown JSON parsing error")) + string_empty}
            }}
        };
    }
};


any isValidCharacter(any data)
{
    return validateCharacter(data)->success;
};


any uuidSchema = z->string()->regex((new RegExp(std:("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$"))), std:("Invalid UUID format"));
any contentSchema = z->object(object{
    object::pair{std:("text"), z->string()->optional()}, 
    object::pair{std:("thought"), z->string()->optional()}, 
    object::pair{std:("actions"), z->array(z->string())->optional()}, 
    object::pair{std:("providers"), z->array(z->string())->optional()}, 
    object::pair{std:("source"), z->string()->optional()}, 
    object::pair{std:("target"), z->string()->optional()}, 
    object::pair{std:("url"), z->string()->optional()}, 
    object::pair{std:("inReplyTo"), uuidSchema->optional()}, 
    object::pair{std:("attachments"), z->array(z->any())->optional()}, 
    object::pair{std:("channelType"), z->string()->optional()}
})->passthrough();
any messageExampleSchema = z->object(object{
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("content"), contentSchema}
});
any directoryItemSchema = z->object(object{
    object::pair{std:("directory"), z->string()}, 
    object::pair{std:("shared"), z->boolean()->optional()}
});
any knowledgeItemSchema = z->union(array<any>{ z->string(), z->object(object{
    object::pair{std:("path"), z->string()}, 
    object::pair{std:("shared"), z->boolean()->optional()}
}), directoryItemSchema });
any templateTypeSchema = z->union(array<any>{ z->string(), z->function()->optional() });
any styleSchema = z->object(object{
    object::pair{std:("all"), z->array(z->string())->optional()}, 
    object::pair{std:("chat"), z->array(z->string())->optional()}, 
    object::pair{std:("post"), z->array(z->string())->optional()}
})->optional();
any settingsSchema = z->record(z->union(array<any>{ z->string(), z->boolean(), z->number(), z->any() }))->optional();
any secretsSchema = z->record(z->union(array<any>{ z->string(), z->boolean(), z->number() }))->optional();
any characterSchema = z->object(object{
    object::pair{std:("id"), uuidSchema->optional()}, 
    object::pair{std:("name"), z->string()->min(1, std:("Character name is required"))}, 
    object::pair{std:("username"), z->string()->optional()}, 
    object::pair{std:("system"), z->string()->optional()}, 
    object::pair{std:("templates"), z->record(templateTypeSchema)->optional()}, 
    object::pair{std:("bio"), z->union(array<any>{ z->string(), z->array(z->string()) })}, 
    object::pair{std:("messageExamples"), z->array(z->array(messageExampleSchema))->optional()}, 
    object::pair{std:("postExamples"), z->array(z->string())->optional()}, 
    object::pair{std:("topics"), z->array(z->string())->optional()}, 
    object::pair{std:("adjectives"), z->array(z->string())->optional()}, 
    object::pair{std:("knowledge"), z->array(knowledgeItemSchema)->optional()}, 
    object::pair{std:("plugins"), z->array(z->string())->optional()}, 
    object::pair{std:("settings"), settingsSchema}, 
    object::pair{std:("secrets"), secretsSchema}, 
    object::pair{std:("style"), styleSchema}
})->strict();

void Main(void)
{
}

MAIN
