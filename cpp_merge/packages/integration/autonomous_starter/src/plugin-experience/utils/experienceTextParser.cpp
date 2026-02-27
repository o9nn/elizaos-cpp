#include "experienceTextParser.hpp"

string detectDomain(string text, string defaultDomain)
{
    auto domains = object{
        object::pair{std:("shell"), array<string>{ std:("command"), std:("terminal"), std:("bash"), std:("shell"), std:("execute"), std:("script"), std:("cli") }}, 
        object::pair{std:("coding"), array<string>{ std:("code"), std:("function"), std:("variable"), std:("syntax"), std:("programming"), std:("debug"), std:("compile") }}, 
        object::pair{std:("system"), array<string>{ std:("file"), std:("directory"), std:("process"), std:("memory"), std:("cpu"), std:("system"), std:("disk") }}, 
        object::pair{std:("network"), array<string>{ std:("http"), std:("api"), std:("request"), std:("response"), std:("url"), std:("network"), std:("server") }}, 
        object::pair{std:("data"), array<string>{ std:("json"), std:("csv"), std:("database"), std:("query"), std:("data"), std:("table"), std:("record") }}, 
        object::pair{std:("plugin"), array<string>{ std:("plugin"), std:("load"), std:("unload"), std:("register"), std:("module"), std:("extension") }}
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


