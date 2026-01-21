#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-personality/src/utils/json-parser.h"

any extractJsonFromResponse(string response)
{
    auto cleaned = response->trim();
    if (cleaned->startsWith(std::string("```json"))) {
        cleaned = cleaned->substring(7);
    } else if (cleaned->startsWith(std::string("```"))) {
        cleaned = cleaned->substring(3);
    }
    if (cleaned->endsWith(std::string("```"))) {
        cleaned = cleaned->substring(0, cleaned->get_length() - 3);
    }
    cleaned = cleaned->trim();
    try
    {
        return JSON->parse(cleaned);
    }
    catch (const any& error)
    {
        auto jsonMatch = response->match((new RegExp(std::string("\{[\s\S]*\"))));
        if (jsonMatch) {
            return JSON->parse((*const_(jsonMatch))[0]);
        }
        throw any(error);
    }
};


