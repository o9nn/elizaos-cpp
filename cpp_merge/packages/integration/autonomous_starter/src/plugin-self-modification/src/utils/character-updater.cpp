#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/utils/character-updater.h"

std::shared_ptr<Character> applyOperationsToCharacter(std::shared_ptr<Character> character, array<std::shared_ptr<ModificationOperation>> operations)
{
    auto updatedCharacter = JSON->parse(JSON->stringify(character));
    for (auto& op : operations)
    {
        try
        {
            static switch_type __switch420_737 = {
                { any(std:("add")), 1 },
                { any(std:("modify")), 2 },
                { any(std:("delete")), 3 }
            };
            switch (__switch420_737[op->type])
            {
            case 1:
                addValue(updatedCharacter, op->path, op->value);
                break;
            case 2:
                modifyValue(updatedCharacter, op->path, op->value);
                break;
            case 3:
                deleteValue(updatedCharacter, op->path);
                break;
            }
        }
        catch (const any& error)
        {
            throw any(std::make_shared<Error>(std:("Failed to apply operation ") + op->type + std:(" at path ") + op->path + std:(": ") + error["message"] + string_empty));
        }
    }
    return updatedCharacter;
};


void addValue(any obj, string path, any value)
{
    auto normalizedPath = (path->startsWith(std:("$"))) ? path : std:("$.") + path + string_empty;
    if (path->includes(std:("[]"))) {
        auto arrayPath = path->replace(std:("[]"), string_empty);
        auto normalizedArrayPath = (arrayPath->startsWith(std:("$"))) ? arrayPath : std:("$.") + arrayPath + string_empty;
        auto results = JSONPath(object{
            object::pair{std:("path"), normalizedArrayPath}, 
            object::pair{std:("json"), obj}
        });
        if (AND((results->length > 0), (Array->isArray(const_(results)[0])))) {
            const_(results)[0]->push(value);
        } else {
            if (arrayPath->includes(std:("."))) {
                auto parentPath = normalizedArrayPath->substring(0, normalizedArrayPath->lastIndexOf(std:(".")));
                auto propertyName = arrayPath->substring(arrayPath->lastIndexOf(std:(".")) + 1);
                auto parentResults = JSONPath(object{
                    object::pair{std:("path"), parentPath}, 
                    object::pair{std:("json"), obj}
                });
                if (parentResults->length > 0) {
                    const_(parentResults)[0][propertyName] = array<any>{ value };
                }
            } else {
                obj[arrayPath] = array<any>{ value };
            }
        }
    } else {
        auto parentPath = normalizedPath->substring(0, normalizedPath->lastIndexOf(std:(".")));
        auto propertyName = normalizedPath->substring(normalizedPath->lastIndexOf(std:(".")) + 1);
        auto parent = const_(JSONPath(object{
            object::pair{std:("path"), parentPath}, 
            object::pair{std:("json"), obj}
        }))[0];
        if (parent) {
            parent[propertyName] = value;
        }
    }
};


void modifyValue(any obj, string path, any value)
{
    auto normalizedPath = (path->startsWith(std:("$"))) ? path : std:("$.") + path + string_empty;
    shared found = false;
    JSONPath(object{
        object::pair{std:("path"), normalizedPath}, 
        object::pair{std:("json"), obj}, 
        object::pair{std:("callback"), [=](any val, any type, any payload) mutable
        {
            if (AND((AND((payload), (payload["parent"]))), (payload["parentProperty"] != undefined))) {
                payload["parent"][payload["parentProperty"]] = value;
                found = true;
            }
        }
        }
    });
    if (!found) {
        throw any(std::make_shared<Error>(std:("Path ") + path + std:(" does not exist")));
    }
};


void deleteValue(any obj, string path)
{
    auto normalizedPath = (path->startsWith(std:("$"))) ? path : std:("$.") + path + string_empty;
    JSONPath(object{
        object::pair{std:("path"), normalizedPath}, 
        object::pair{std:("json"), obj}, 
        object::pair{std:("callback"), [=](any val, any type, any payload) mutable
        {
            if (AND((payload), (payload["parent"]))) {
                if (Array->isArray(payload["parent"])) {
                    auto index = parseInt(payload["parentProperty"]);
                    if (!isNaN(index)) {
                        payload["parent"]["splice"](index, 1);
                    }
                } else {
                    payload["parent"].Delete(payload["parentProperty"]);
                }
            }
        }
        }
    });
};


boolean validateCharacterStructure(any character)
{
    if (OR((!character["name"]), (type_of(character["name"]) != std:("string")))) {
        return false;
    }
    auto arrayFields = array<string>{ std:("bio"), std:("lore"), std:("messageExamples"), std:("postExamples"), std:("topics"), std:("adjectives") };
    for (auto& field : arrayFields)
    {
        if (AND((const_(character)[field]), (!Array->isArray(const_(character)[field])))) {
            if (AND((field == std:("bio")), (type_of(const_(character)[field]) == std:("string")))) {
                continue;
            }
            return false;
        }
    }
    if (character["style"]) {
        if (type_of(character["style"]) != std:("object")) {
            return false;
        }
        auto styleFields = array<string>{ std:("all"), std:("chat"), std:("post") };
        for (auto& field : styleFields)
        {
            if (AND((const_(character["style"])[field]), (!Array->isArray(const_(character["style"])[field])))) {
                return false;
            }
        }
    }
    return true;
};


