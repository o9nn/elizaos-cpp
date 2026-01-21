#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/agent/utils/yaml.h"

object parseMultilineString(array<string> lines, double startIndex, double expectedIndent)
{
    auto result = array<string>();
    auto i = startIndex;
    while (i < lines->get_length())
    {
        auto line = const_(lines)[i];
        auto indent = line->get_length() - line->trimStart()->length;
        if (AND((line->trim()), (indent < expectedIndent))) {
            break;
        }
        if (indent >= expectedIndent) {
            result->push(line->substring(expectedIndent));
        } else if (!line->trim()) {
            result->push(string_empty);
        } else {
            break;
        }
        i++;
    }
    return object{
        object::pair{std::string("value"), result->join(std::string("\
"))->trimEnd()}, 
        object::pair{std::string("nextIndex"), i}
    };
};


Record<string, any> parseKeyValue(string str)
{
    auto result = object{};
    auto pairs = str->split(std::string(", "));
    for (auto& pair : pairs)
    {
        if (pair->includes(std::string(": "))) {
            auto [key, value] = pair->split(std::string(": "));
            result[key->trim()] = parseValue(value->trim());
        }
    }
    return result;
};


