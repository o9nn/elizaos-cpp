#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/utils/xml-parser.h"

string sanitizeXml(string xmlString)
{
    auto sanitized = xmlString->replace((new RegExp(std:("<!DOCTYPE[^>]*>/"))), string_empty);
    sanitized = sanitized->replace((new RegExp(std:("<!ENTITY[^>]*>/"))), string_empty);
    sanitized = sanitized->replace((new RegExp(std:("<\?(?!xml)[^>]*\?>/"))), string_empty);
    sanitized = sanitized->replace((new RegExp(std:("<!\[CDATA\[[\s\S]*?\]\]>/"))), [=](auto match) mutable
    {
        auto content = match->slice(9, -3);
        return escapeXml(content);
    }
    );
    return sanitized;
};


string escapeXml(string unsafe)
{
    return unsafe->replace((new RegExp(std:("&"))), std:("&amp;"))->replace((new RegExp(std:("<"))), std:("&lt;"))->replace((new RegExp(std:(">"))), std:("&gt;"))->replace((new RegExp(std:("""))), std:("&quot;"))->replace((new RegExp(std:("'"))), std:("&#039;"));
};


any isValidOperationType(string type)
{
    return (array<string>{ std:("add"), std:("modify"), std:("delete") })->includes(type);
};


std::shared_ptr<CharacterDiff> parseCharacterDiff(string xmlString)
{
    try
    {
        auto sanitizedXml = sanitizeXml(xmlString);
        auto parsed = parser->parse(sanitizedXml);
        auto root = const_(parsed)[std:("character-modification")];
        if (!root) {
            throw any(std::make_shared<Error>(std:("Invalid XML: missing character-modification root element")));
        }
        shared operations = array<std::shared_ptr<ModificationOperation>>();
        auto opsRoot = root->operations;
        if (opsRoot) {
            auto processOperation = [=](auto op, auto type) mutable
            {
                if (!isValidOperationType(type)) {
                    throw any(std::make_shared<Error>(std:("Invalid operation type: ") + type + string_empty));
                }
                auto items = (Array->isArray(op)) ? op : array<any>{ op };
                items->forEach([=](auto item) mutable
                {
                    auto path = const_(item)[std:("@_path")];
                    if (OR((!path), (type_of(path) != std:("string")))) {
                        throw any(std::make_shared<Error>(std:("Invalid path in ") + type + std:(" operation")));
                    }
                    if (OR((path->includes(std:(".."))), (path->includes(std:("//"))))) {
                        throw any(std::make_shared<Error>(std:("Dangerous path pattern detected: ") + path + string_empty));
                    }
                    auto operation = object{
                        object::pair{std:("type"), std:("type")}, 
                        object::pair{std:("path"), std:("path")}
                    };
                    if (type != std:("delete")) {
                        operation->value = OR((const_(item)[std:("#text")]), (item));
                        operation->dataType = const_(item)[std:("@_type")];
                    }
                    operations->push(operation);
                }
                );
            };
            auto validOps = array<string>{ std:("add"), std:("modify"), std:("delete") };
            auto opsKeys = Object->keys(opsRoot);
            for (auto& key : opsKeys)
            {
                if (!validOps->includes(key)) {
                    throw any(std::make_shared<Error>(std:("Invalid operation type: ") + key + string_empty));
                }
            }
            if (opsRoot->add) processOperation(opsRoot->add, std:("add"));
            if (opsRoot->modify) processOperation(opsRoot->modify, std:("modify"));
            if (opsRoot->delete) processOperation(opsRoot->delete, std:("delete"));
        }
        auto reasoning = root->reasoning;
        if (OR((OR((!reasoning), (type_of(reasoning) != std:("string")))), (reasoning->trim()->get_length() == 0))) {
            throw any(std::make_shared<Error>(std:("Missing or empty reasoning in character modification")));
        }
        return object{
            object::pair{std:("operations"), std:("operations")}, 
            object::pair{std:("reasoning"), reasoning->trim()}, 
            object::pair{std:("timestamp"), OR((root->timestamp), (((std::make_shared<Date>()))->toISOString()))}
        };
    }
    catch (const any& error)
    {
        throw any(std::make_shared<Error>(std:("Failed to parse character diff XML: ") + (is<Error>(error)) ? error->message : String(error) + string_empty));
    }
};


string buildCharacterDiffXml(std::shared_ptr<CharacterDiff> diff)
{
    if (OR((!diff->reasoning), (diff->reasoning->trim()->length == 0))) {
        throw any(std::make_shared<Error>(std:("Reasoning is required for character modifications")));
    }
    if (!Array->isArray(diff->operations)) {
        throw any(std::make_shared<Error>(std:("Operations must be an array")));
    }
    for (auto& op : diff->operations)
    {
        if (OR((!op->path), (type_of(op->path) != std:("string")))) {
            throw any(std::make_shared<Error>(std:("Invalid path in operation: ") + JSON->stringify(op) + string_empty));
        }
    }
    auto xmlObj = object{
        object::pair{std:("character-modification"), object{
            object::pair{std:("operations"), object{
                object::pair{std:("add"), diff->operations->filter([=](auto op) mutable
                {
                    return op["type"] == std:("add");
                }
                )->map([=](auto op) mutable
                {
                    return (object{
                        object::pair{std:("@_path"), op["path"]}, 
                        object::pair{std:("@_type"), OR((op["dataType"]), (std:("string")))}, 
                        object::pair{std:("#text"), op["value"]}
                    });
                }
                )}, 
                object::pair{std:("modify"), diff->operations->filter([=](auto op) mutable
                {
                    return op["type"] == std:("modify");
                }
                )->map([=](auto op) mutable
                {
                    return (object{
                        object::pair{std:("@_path"), op["path"]}, 
                        object::pair{std:("@_type"), OR((op["dataType"]), (std:("string")))}, 
                        object::pair{std:("#text"), op["value"]}
                    });
                }
                )}, 
                object::pair{std:("delete"), diff->operations->filter([=](auto op) mutable
                {
                    return op["type"] == std:("delete");
                }
                )->map([=](auto op) mutable
                {
                    return (object{
                        object::pair{std:("@_path"), op["path"]}
                    });
                }
                )}
            }}, 
            object::pair{std:("reasoning"), diff->reasoning}, 
            object::pair{std:("timestamp"), diff->timestamp}
        }}
    };
    auto ops = const_(xmlObj)[std:("character-modification")]["operations"];
    if (ops["add"]->length == 0) ops.Delete("add");
    if (ops["modify"]->length == 0) ops.Delete("modify");
    if (ops["delete"]->length == 0) ops.Delete("delete");
    try
    {
        return builder->build(xmlObj);
    }
    catch (const any& error)
    {
        throw any(std::make_shared<Error>(std:("Failed to build character diff XML: ") + (is<Error>(error)) ? error->message : String(error) + string_empty));
    }
};


any parser = std::make_shared<XMLParser>(object{
    object::pair{std:("ignoreAttributes"), false}, 
    object::pair{std:("attributeNamePrefix"), std:("@_")}, 
    object::pair{std:("parseAttributeValue"), true}, 
    object::pair{std:("trimValues"), true}, 
    object::pair{std:("processEntities"), false}, 
    object::pair{std:("stopNodes"), array<string>{ std:("*.script"), std:("*.style") }}, 
    object::pair{std:("parseTagValue"), true}
});
any builder = std::make_shared<XMLBuilder>(object{
    object::pair{std:("ignoreAttributes"), false}, 
    object::pair{std:("attributeNamePrefix"), std:("@_")}, 
    object::pair{std:("format"), true}, 
    object::pair{std:("indentBy"), std:("  ")}, 
    object::pair{std:("suppressEmptyNode"), true}, 
    object::pair{std:("suppressBooleanAttributes"), false}
});

void Main(void)
{
}

MAIN
