#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_AGENT_UTILS_YAML_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_AGENT_UTILS_YAML_H
#include "core.hpp"

typedef any YamlData;


template <typename RET>
RET parseYAML(string yamlString);

object parseMultilineString(array<string> lines, double startIndex, double expectedIndent);

Record<string, any> parseKeyValue(string str);

template <typename RET>
RET parseValue(string value);

template <typename P0>
string stringifyYAML(P0 obj, double indent = 0);

template <typename RET>
RET parseYAML(string yamlString)
{
    auto lines = yamlString->split(std:("\
"));
    auto result = object{};
    auto stack = array<any>{ result };
    auto indentStack = array<double>{ 0 };
    auto currentList = nullptr;
    auto currentListIndent = -1;
    for (auto i = 0; i < lines->get_length(); i++)
    {
        auto line = const_(lines)[i];
        auto trimmedLine = line->trim();
        if (OR((!trimmedLine), (trimmedLine->startsWith(std:("#"))))) {
            continue;
        }
        auto indent = line->get_length() - line->trimStart()->length;
        if (trimmedLine->startsWith(std:("- "))) {
            auto value = trimmedLine->substring(2)->trim();
            while (AND((indentStack->get_length() > 1), (indent <= const_(indentStack)[indentStack->get_length() - 1])))
            {
                stack->pop();
                indentStack->pop();
                if (currentListIndent >= const_(indentStack)[indentStack->get_length() - 1]) {
                    currentList = nullptr;
                    currentListIndent = -1;
                }
            }
            if (OR((!currentList), (indent != currentListIndent))) {
                currentList = array<any>();
                currentListIndent = indent;
                auto parent = const_(stack)[stack->get_length() - 1];
                auto lastKey = Object->keys(parent)->pop();
                if (AND((AND((lastKey), (!Array->isArray(parent)))), ((*const_(parent))[lastKey] == nullptr))) {
                    (*parent)[lastKey] = currentList;
                } else {
                    if (!Array->isArray(parent)) {
                        (*parent)[std:("items")] = currentList;
                    }
                }
            }
            if (value->includes(std:(": "))) {
                auto obj = parseKeyValue(value);
                currentList->push(obj);
            } else {
                currentList->push(parseValue(value));
            }
        } else if (trimmedLine->includes(std:(": "))) {
            auto colonIndex = trimmedLine->indexOf(std:(": "));
            auto key = trimmedLine->substring(0, colonIndex)->trim();
            auto value = trimmedLine->substring(colonIndex + 2)->trim();
            while (AND((indentStack->get_length() > 1), (indent < const_(indentStack)[indentStack->get_length() - 1])))
            {
                stack->pop();
                indentStack->pop();
            }
            auto parent = const_(stack)[stack->get_length() - 1];
            if (OR((OR((!value), (value == std:("|")))), (value == std:(">")))) {
                if (OR((value == std:("|")), (value == std:(">")))) {
                    auto multilineValue = parseMultilineString(lines, i + 1, indent + 2);
                    if (!Array->isArray(parent)) {
                        (*parent)[key] = multilineValue["value"];
                    }
                    i = multilineValue["nextIndex"] - 1;
                } else {
                    if (!Array->isArray(parent)) {
                        (*parent)[key] = object{};
                        stack->push((*const_(parent))[key]);
                    }
                    indentStack->push(indent);
                }
            } else {
                if (!Array->isArray(parent)) {
                    (*parent)[key] = parseValue(value);
                }
            }
            if (indent <= currentListIndent) {
                currentList = nullptr;
                currentListIndent = -1;
            }
        }
    }
    return result;
};


template <typename RET>
RET parseValue(string value)
{
    if (OR((OR((value == std:("null")), (value == std:("~")))), (value == string_empty))) {
        return nullptr;
    }
    if (OR((OR((value == std:("true")), (value == std:("yes")))), (value == std:("on")))) {
        return true;
    }
    if (OR((OR((value == std:("false")), (value == std:("no")))), (value == std:("off")))) {
        return false;
    }
    if (AND((!isNaN(Number(value))), (value != string_empty))) {
        if (value->includes(std:("."))) {
            return parseFloat(value);
        }
        return parseInt(value, 10);
    }
    if (OR(((AND((value->startsWith(std:("""))), (value->endsWith(std:(""")))))), ((AND((value->startsWith(std:("'"))), (value->endsWith(std:("'")))))))) {
        return value->slice(1, -1);
    }
    if (AND((value->startsWith(std:("["))), (value->endsWith(std:("]"))))) {
        auto items = value->slice(1, -1)->split(std:(","))->map([=](auto item) mutable
        {
            return parseValue(item->trim());
        }
        );
        return items;
    }
    if (AND((value->startsWith(std:("{"))), (value->endsWith(std:("}"))))) {
        auto obj = object{};
        auto pairs = value->slice(1, -1)->split(std:(","));
        for (auto& pair : pairs)
        {
            auto [key, val] = pair->split(std:(":"))->map([=](auto s) mutable
            {
                return s->trim();
            }
            );
            obj[key] = parseValue(val);
        }
        return obj;
    }
    return value;
};


template <typename P0>
string stringifyYAML(P0 obj, double indent)
{
    shared lines = array<string>();
    auto indentStr = std:("  ")->repeat(indent);
    if (Array->isArray(obj)) {
        for (auto& item : obj)
        {
            if (AND((type_of(item) == std:("object")), (item != nullptr))) {
                lines->push(string_empty + indentStr + std:("- ") + stringifyYAML(item, 0)->trim() + string_empty);
            } else {
                lines->push(string_empty + indentStr + std:("- ") + item + string_empty);
            }
        }
    } else if (AND((type_of(obj) == std:("object")), (obj != nullptr))) {
        for (auto& [key, value] : Object->entries(obj))
        {
            if (OR((value == nullptr), (value == undefined))) {
                lines->push(string_empty + indentStr + string_empty + key + std:(":"));
            } else if (Array->isArray(value)) {
                lines->push(string_empty + indentStr + string_empty + key + std:(":"));
                lines->push(stringifyYAML(value, indent + 1));
            } else if (type_of(value) == std:("object")) {
                lines->push(string_empty + indentStr + string_empty + key + std:(":"));
                lines->push(stringifyYAML(value, indent + 1));
            } else if (AND((type_of(value) == std:("string")), (value->includes(std:("\
"))))) {
                lines->push(string_empty + indentStr + string_empty + key + std:(": |"));
                value->split(std:("\
"))->forEach([=](auto line) mutable
                {
                    lines->push(string_empty + std:("  ")->repeat(indent + 1) + string_empty + line + string_empty);
                }
                );
            } else {
                lines->push(string_empty + indentStr + string_empty + key + std:(": ") + value + string_empty);
            }
        }
    } else {
        return String(obj);
    }
    return lines->join(std:("\
"));
};


#endif
