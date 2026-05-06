#include "experienceEvaluator.hpp"
#include <string>

string sanitizeContext(string text)
{
    if (!text) return std::string("Unknown context");
    return text->replace((new RegExp(std::string("\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b"))), std::string("[EMAIL]"))->replace((new RegExp(std::string("\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b"))), std::string("[IP]"))->replace((new RegExp(std::string("\/Users\/[^\/\s]+"))), std::string("/Users/[USER]"))->replace((new RegExp(std::string("\/home\/[^\/\s]+"))), std::string("/home/[USER]"))->replace((new RegExp(std::string("\b[A-Z0-9]{20,}\b"))), std::string("[TOKEN]"))->replace((new RegExp(std::string("\b(user|person|someone|they)\s+(said|asked|told|mentioned)/"))), std::string("when asked"))->substring(0, 200);
};


string detectDomain(string text)
{
    auto domains = object{
        object::pair{std::string("shell"), array<string>{ std::string("command"), std::string("terminal"), std::string("bash"), std::string("shell"), std::string("execute"), std::string("script"), std::string("cli") }}, 
        object::pair{std::string("coding"), array<string>{ std::string("code"), std::string("function"), std::string("variable"), std::string("syntax"), std::string("programming"), std::string("debug"), std::string("typescript"), std::string("javascript") }}, 
        object::pair{std::string("system"), array<string>{ std::string("file"), std::string("directory"), std::string("process"), std::string("memory"), std::string("cpu"), std::string("system"), std::string("install"), std::string("package") }}, 
        object::pair{std::string("network"), array<string>{ std::string("http"), std::string("api"), std::string("request"), std::string("response"), std::string("url"), std::string("network"), std::string("fetch"), std::string("curl") }}, 
        object::pair{std::string("data"), array<string>{ std::string("json"), std::string("csv"), std::string("database"), std::string("query"), std::string("data"), std::string("sql"), std::string("table") }}, 
        object::pair{std::string("ai"), array<string>{ std::string("model"), std::string("llm"), std::string("embedding"), std::string("prompt"), std::string("token"), std::string("inference") }}
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
    return std::string("general");
};


std::shared_ptr<Evaluator> experienceEvaluator = object{
    object::pair{std::string("name"), std::string("EXPERIENCE_EVALUATOR")}, 
    object::pair{std::string("similes"), array<string>{ std::string("experience recorder"), std::string("learning evaluator"), std::string("self-reflection") }}, 
    object::pair{std::string("description"), std::string("Periodically analyzes conversation patterns to extract novel learning experiences")}, 
    object::pair{std::string("alwaysRun"), false}, 
    object::pair{std::string("examples"), array<object>{ object{
        object::pair{std::string("prompt"), std::string("The agent successfully executed a shell command after initially failing")}, 
        object::pair{std::string("messages"), array<object>{ object{
            object::pair{std::string("name"), std::string("Autoliza")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("Let me try to run this Python script.")}
            }}
        }, object{
            object::pair{std::string("name"), std::string("Autoliza")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("Error: ModuleNotFoundError for pandas. I need to install it first.")}
            }}
        }, object{
            object::pair{std::string("name"), std::string("Autoliza")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("After installing pandas, the script ran successfully and produced the expected output.")}
            }}
        } }}, 
        object::pair{std::string("outcome"), std::string("Record a CORRECTION experience about needing to install dependencies before running Python scripts")}
    }, object{
        object::pair{std::string("prompt"), std::string("The agent discovered a new system capability")}, 
        object::pair{std::string("messages"), array<object>{ object{
            object::pair{std::string("name"), std::string("Autoliza")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("I found that the system has jq installed, which is perfect for parsing JSON data.")}
            }}
        } }}, 
        object::pair{std::string("outcome"), std::string("Record a DISCOVERY experience about the availability of jq for JSON processing")}
    } }}, 
    , 
};

void Main(void)
{
}

MAIN
