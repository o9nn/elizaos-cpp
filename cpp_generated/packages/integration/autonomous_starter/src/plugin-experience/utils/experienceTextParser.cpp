#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/utils/experienceTextParser.h"

string detectDomain(string text, string defaultDomain)
{
    auto domains = object{
        object::pair{std::string("shell"), array<string>{ std::string("command"), std::string("terminal"), std::string("bash"), std::string("shell"), std::string("execute"), std::string("script"), std::string("cli") }}, 
        object::pair{std::string("coding"), array<string>{ std::string("code"), std::string("function"), std::string("variable"), std::string("syntax"), std::string("programming"), std::string("debug"), std::string("compile") }}, 
        object::pair{std::string("system"), array<string>{ std::string("file"), std::string("directory"), std::string("process"), std::string("memory"), std::string("cpu"), std::string("system"), std::string("disk") }}, 
        object::pair{std::string("network"), array<string>{ std::string("http"), std::string("api"), std::string("request"), std::string("response"), std::string("url"), std::string("network"), std::string("server") }}, 
        object::pair{std::string("data"), array<string>{ std::string("json"), std::string("csv"), std::string("database"), std::string("query"), std::string("data"), std::string("table"), std::string("record") }}, 
        object::pair{std::string("plugin"), array<string>{ std::string("plugin"), std::string("load"), std::string("unload"), std::string("register"), std::string("module"), std::string("extension") }}
    };
    shared lowerText = text->toLowerCase();
    for (auto& [domain, keywords] : Object->entries(domains))
    {
        if (keywords->some([=](auto keyword) mutable
        {
            return lowerText->includes(keyword);
        }
        )) {
            return domain;
        }
    }
    return defaultDomain;
};


