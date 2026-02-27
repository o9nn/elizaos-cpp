#include "xml-parser.h"
#include <string>

string sanitizeXml(string xmlString)
{
    auto sanitized = xmlString->replace((new RegExp(std::string("<!DOCTYPE[^>]*>/"))), string_empty);
    sanitized = sanitized->replace((new RegExp(std::string("<!ENTITY[^>]*>/"))), string_empty);
    sanitized = sanitized->replace((new RegExp(std::string("<\?(?!xml)[^>]*\?>/"))), string_empty);
    sanitized = sanitized->replace((new RegExp(std::string("<!\[CDATA\[[\s\S]*?\]\]>/"))), [=](auto match) mutable
    {
        auto content = match->slice(9, -3);
        return escapeXml(content);
    }
    );
    return sanitized;
};


string escapeXml(string unsafe)
{
    return unsafe->replace((new RegExp(std::string("&"))), std::string("&amp;"))->replace((new RegExp(std::string("<"))), std::string("&lt;"))->replace((new RegExp(std::string(">"))), std::string("&gt;"))->replace((new RegExp(std::string("""))), std::string("&quot;"))->replace((new RegExp(std::string("'"))), std::string("&#039;"));
};


any isValidOperationType(string type)
{
    return (array<string>{ std::string("add"), std::string("modify"), std::string("delete") })->includes(type);
};


std::shared_ptr<CharacterDiff> parseCharacterDiff(string xmlString)
{
    try
    {
        auto sanitizedXml = sanitizeXml(xmlString);
        auto parsed = parser->parse(sanitizedXml);
        auto root = const_(parsed)[std::string("character-modification")];
        if (!root) {
            throw any(std::make_shared<Error>(std::string("Invalid XML: missing character-modification root element")));
        }
        shared operations = array<std::shared_ptr<ModificationOperation>>();
        auto opsRoot = root->operations;
        if (opsRoot) {
            auto processOperation = [=](auto op, auto type) mutable
            {
                if (!isValidOperationType(type)) {
                    throw any(std::make_shared<Error>(std::string("Invalid operation type: ") + type + string_empty));
                }
                auto items = (Array->isArray(op)) ? op : array<any>{ op };
                items->forEach([=](auto item) mutable
                {
                    auto path = const_(item)[std::string("@_path")];
                    if (OR((!path), (type_of(path) != std::string("string")))) {
                        throw any(std::make_shared<Error>(std::string("Invalid path in ") + type + std::string(" operation")));
                    }
                    if (OR((path->includes(std::string(".."))), (path->includes(std::string("//"))))) {
                        throw any(std::make_shared<Error>(std::string("Dangerous path pattern detected: ") + path + string_empty));
                    }
                    auto operation = object{
                        object::pair{std::string("type"), std::string("type")}, 
                        object::pair{std::string("path"), std::string("path")}
                    };
                    if (type != std::string("delete")) {
                        operation->value = OR((const_(item)[std::string("#text")]), (item));
                        operation->dataType = const_(item)[std::string("@_type")];
                    }
                    operations->push(operation);
                }
                );
            };
            auto validOps = array<string>{ std::string("add"), std::string("modify"), std::string("delete") };
            auto opsKeys = Object->keys(opsRoot);
            for (auto& key : opsKeys)
            {
                if (!validOps->includes(key)) {
                    throw any(std::make_shared<Error>(std::string("Invalid operation type: ") + key + string_empty));
                }
            }
            if (opsRoot->add) processOperation(opsRoot->add, std::string("add"));
            if (opsRoot->modify) processOperation(opsRoot->modify, std::string("modify"));
            if (opsRoot->delete) processOperation(opsRoot->delete, std::string("delete"));
        }
        auto reasoning = root->reasoning;
        if (OR((OR((!reasoning), (type_of(reasoning) != std::string("string")))), (reasoning->trim()->get_length() == 0))) {
            throw any(std::make_shared<Error>(std::string("Missing or empty reasoning in character modification")));
        }
        return object{
            object::pair{std::string("operations"), std::string("operations")}, 
            object::pair{std::string("reasoning"), reasoning->trim()}, 
            object::pair{std::string("timestamp"), OR((root->timestamp), (((std::make_shared<Date>()))->toISOString()))}
        };
    }
    catch (const any& error)
    {
        throw any(std::make_shared<Error>(std::string("Failed to parse character diff XML: ") + (is<Error>(error)) ? error->message : String(error) + string_empty));
    }
};


string buildCharacterDiffXml(std::shared_ptr<CharacterDiff> diff)
{
    if (OR((!diff->reasoning), (diff->reasoning->trim()->length == 0))) {
        throw any(std::make_shared<Error>(std::string("Reasoning is required for character modifications")));
    }
    if (!Array->isArray(diff->operations)) {
        throw any(std::make_shared<Error>(std::string("Operations must be an array")));
    }
    for (auto& op : diff->operations)
    {
        if (OR((!op->path), (type_of(op->path) != std::string("string")))) {
            throw any(std::make_shared<Error>(std::string("Invalid path in operation: ") + JSON->stringify(op) + string_empty));
        }
    }
    auto xmlObj = object{
        object::pair{std::string("character-modification"), object{
            object::pair{std::string("operations"), object{
                object::pair{std::string("add"), diff->operations->filter([=](auto op) mutable
                {
                    return op["type"] == std::string("add");
                }
                )->map([=](auto op) mutable
                {
                    return (object{
                        object::pair{std::string("@_path"), op["path"]}, 
                        object::pair{std::string("@_type"), OR((op["dataType"]), (std::string("string")))}, 
                        object::pair{std::string("#text"), op["value"]}
                    });
                }
                )}, 
                object::pair{std::string("modify"), diff->operations->filter([=](auto op) mutable
                {
                    return op["type"] == std::string("modify");
                }
                )->map([=](auto op) mutable
                {
                    return (object{
                        object::pair{std::string("@_path"), op["path"]}, 
                        object::pair{std::string("@_type"), OR((op["dataType"]), (std::string("string")))}, 
                        object::pair{std::string("#text"), op["value"]}
                    });
                }
                )}, 
                object::pair{std::string("delete"), diff->operations->filter([=](auto op) mutable
                {
                    return op["type"] == std::string("delete");
                }
                )->map([=](auto op) mutable
                {
                    return (object{
                        object::pair{std::string("@_path"), op["path"]}
                    });
                }
                )}
            }}, 
            object::pair{std::string("reasoning"), diff->reasoning}, 
            object::pair{std::string("timestamp"), diff->timestamp}
        }}
    };
    auto ops = const_(xmlObj)[std::string("character-modification")]["operations"];
    if (ops["add"]->length == 0) ops.Delete("add");
    if (ops["modify"]->length == 0) ops.Delete("modify");
    if (ops["delete"]->length == 0) ops.Delete("delete");
    try
    {
        return builder->build(xmlObj);
    }
    catch (const any& error)
    {
        throw any(std::make_shared<Error>(std::string("Failed to build character diff XML: ") + (is<Error>(error)) ? error->message : String(error) + string_empty));
    }
};


any parser = std::make_shared<XMLParser>(object{
    object::pair{std::string("ignoreAttributes"), false}, 
    object::pair{std::string("attributeNamePrefix"), std::string("@_")}, 
    object::pair{std::string("parseAttributeValue"), true}, 
    object::pair{std::string("trimValues"), true}, 
    object::pair{std::string("processEntities"), false}, 
    object::pair{std::string("stopNodes"), array<string>{ std::string("*.script"), std::string("*.style") }}, 
    object::pair{std::string("parseTagValue"), true}
});
any builder = std::make_shared<XMLBuilder>(object{
    object::pair{std::string("ignoreAttributes"), false}, 
    object::pair{std::string("attributeNamePrefix"), std::string("@_")}, 
    object::pair{std::string("format"), true}, 
    object::pair{std::string("indentBy"), std::string("  ")}, 
    object::pair{std::string("suppressEmptyNode"), true}, 
    object::pair{std::string("suppressBooleanAttributes"), false}
});

void Main(void)
{
}

MAIN
