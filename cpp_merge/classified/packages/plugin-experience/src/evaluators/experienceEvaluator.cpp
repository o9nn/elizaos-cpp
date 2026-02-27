#include "experienceEvaluator.hpp"

string sanitizeContext(string text)
{
    if (!text) return std:("Unknown context");
    return text->replace((new RegExp(std:("\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b"))), std:("[EMAIL]"))->replace((new RegExp(std:("\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b"))), std:("[IP]"))->replace((new RegExp(std:("\/Users\/[^\/\s]+"))), std:("/Users/[USER]"))->replace((new RegExp(std:("\/home\/[^\/\s]+"))), std:("/home/[USER]"))->replace((new RegExp(std:("\b[A-Z0-9]{20,}\b"))), std:("[TOKEN]"))->replace((new RegExp(std:("\b(user|person|someone|they)\s+(said|asked|told|mentioned)/"))), std:("when asked"))->substring(0, 200);
};


string detectDomain(string text)
{
    auto domains = object{
        object::pair{std:("shell"), array<string>{ std:("command"), std:("terminal"), std:("bash"), std:("shell"), std:("execute"), std:("script"), std:("cli") }}, 
        object::pair{std:("coding"), array<string>{ std:("code"), std:("function"), std:("variable"), std:("syntax"), std:("programming"), std:("debug"), std:("typescript"), std:("javascript") }}, 
        object::pair{std:("system"), array<string>{ std:("file"), std:("directory"), std:("process"), std:("memory"), std:("cpu"), std:("system"), std:("install"), std:("package") }}, 
        object::pair{std:("network"), array<string>{ std:("http"), std:("api"), std:("request"), std:("response"), std:("url"), std:("network"), std:("fetch"), std:("curl") }}, 
        object::pair{std:("data"), array<string>{ std:("json"), std:("csv"), std:("database"), std:("query"), std:("data"), std:("sql"), std:("table") }}, 
        object::pair{std:("ai"), array<string>{ std:("model"), std:("llm"), std:("embedding"), std:("prompt"), std:("token"), std:("inference") }}
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
    return std:("general");
};


std::shared_ptr<Evaluator> experienceEvaluator = object{
    object::pair{std:("name"), std:("EXPERIENCE_EVALUATOR")}, 
    object::pair{std:("similes"), array<string>{ std:("experience recorder"), std:("learning evaluator"), std:("self-reflection") }}, 
    object::pair{std:("description"), std:("Periodically analyzes conversation patterns to extract novel learning experiences")}, 
    object::pair{std:("alwaysRun"), false}, 
    object::pair{std:("examples"), array<object>{ object{
        object::pair{std:("prompt"), std:("The agent successfully executed a shell command after initially failing")}, 
        object::pair{std:("messages"), array<object>{ object{
            object::pair{std:("name"), std:("Autoliza")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("Let me try to run this Python script.")}
            }}
        }, object{
            object::pair{std:("name"), std:("Autoliza")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("Error: ModuleNotFoundError for pandas. I need to install it first.")}
            }}
        }, object{
            object::pair{std:("name"), std:("Autoliza")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("After installing pandas, the script ran successfully and produced the expected output.")}
            }}
        } }}, 
        object::pair{std:("outcome"), std:("Record a CORRECTION experience about needing to install dependencies before running Python scripts")}
    }, object{
        object::pair{std:("prompt"), std:("The agent discovered a new system capability")}, 
        object::pair{std:("messages"), array<object>{ object{
            object::pair{std:("name"), std:("Autoliza")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("I found that the system has jq installed, which is perfect for parsing JSON data.")}
            }}
        } }}, 
        object::pair{std:("outcome"), std:("Record a DISCOVERY experience about the availability of jq for JSON processing")}
    } }}, 
    , 
};

void Main(void)
{
}

MAIN
