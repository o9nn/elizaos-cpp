#include "experienceEvaluator.hpp"

string extractContext(array<std::shared_ptr<Memory>> messages)
{
    if (OR((!messages), (messages->get_length() == 0))) return std:("Unknown context");
    auto recentMessages = messages->slice(-3);
    return recentMessages->map([=](auto m) mutable
    {
        return m->content->text;
    }
    )->filter(Boolean)->join(std:(" -> "));
};


string extractAction(string text)
{
    auto actionPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("trying to (.+?)(?:\.|,|$)"))), (new RegExp(std:("attempted to (.+?)(?:\.|,|$)"))), (new RegExp(std:("executed (.+?)(?:\.|,|$)"))), (new RegExp(std:("ran (.+?)(?:\.|,|$)"))), (new RegExp(std:("performed (.+?)(?:\.|,|$)"))) };
    for (auto& pattern : actionPatterns)
    {
        auto match = text->match(pattern);
        if (match) return (*const_(match))[1]->trim();
    }
    return std:("performed action");
};


string extractError(string text)
{
    auto errorMatch = text->match((new RegExp(std:("error:?\s*(.+?)(?:\.|$)"))));
    if (errorMatch) return (*const_(errorMatch))[1]->trim();
    auto failedMatch = text->match((new RegExp(std:("failed:?\s*(.+?)(?:\.|$)"))));
    if (failedMatch) return (*const_(failedMatch))[1]->trim();
    return std:("encountered error");
};


string extractDiscovery(string text)
{
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("found (?:that )?(.+?)(?:\.|,|$)"))), (new RegExp(std:("discovered (?:that )?(.+?)(?:\.|,|$)"))), (new RegExp(std:("realized (?:that )?(.+?)(?:\.|,|$)"))), (new RegExp(std:("noticed (?:that )?(.+?)(?:\.|,|$)"))) };
    for (auto& pattern : patterns)
    {
        auto match = text->match(pattern);
        if (match) return (*const_(match))[1]->trim();
    }
    return std:("made a discovery");
};


string extractLearning(string text, string type)
{
    auto learningMatch = text->match((new RegExp(std:("(?:learned|learning|lesson):?\s*(.+?)(?:\.|$)"))));
    if (learningMatch) return (*const_(learningMatch))[1]->trim();
    static switch_type __switch14928_15422 = {
        { any(std:("correction")), 1 },
        { any(std:("discovery")), 2 },
        { any(std:("success")), 3 }
    };
    switch (__switch14928_15422[type])
    {
    case 1:
        return std:("Corrected approach works better than initial attempt");
    case 2:
        auto discovery = extractDiscovery(text);
        return (discovery != std:("made a discovery")) ? any(discovery) (std:("New capability or information discovered"));
    case 3:
        return std:("This approach successfully completes the task");
    default:
        return std:("Experience recorded for future reference");
    }
};


string extractHypothesis(string text)
{
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("i (?:think|believe) (?:that )?(.+?)(?:\.|$)"))), (new RegExp(std:("hypothesis:?\s*(.+?)(?:\.|$)"))), (new RegExp(std:("theory:?\s*(.+?)(?:\.|$)"))) };
    for (auto& pattern : patterns)
    {
        auto match = text->match(pattern);
        if (match) return (*const_(match))[1]->trim();
    }
    return std:("formed hypothesis");
};


string detectDomain(string text)
{
    auto domains = object{
        object::pair{std:("shell"), array<string>{ std:("command"), std:("terminal"), std:("bash"), std:("shell"), std:("execute"), std:("script") }}, 
        object::pair{std:("coding"), array<string>{ std:("code"), std:("function"), std:("variable"), std:("syntax"), std:("programming"), std:("debug") }}, 
        object::pair{std:("system"), array<string>{ std:("file"), std:("directory"), std:("process"), std:("memory"), std:("cpu"), std:("system") }}, 
        object::pair{std:("network"), array<string>{ std:("http"), std:("api"), std:("request"), std:("response"), std:("url"), std:("network") }}, 
        object::pair{std:("data"), array<string>{ std:("json"), std:("csv"), std:("database"), std:("query"), std:("data") }}
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
    object::pair{std:("description"), std:("Evaluates agent actions and outcomes to record significant experiences and learnings")}, 
    object::pair{std:("alwaysRun"), true}, 
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
