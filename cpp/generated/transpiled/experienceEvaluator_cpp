#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/evaluators/experienceEvaluator.h"

string extractContext(array<std::shared_ptr<Memory>> messages)
{
    if (OR((!messages), (messages->get_length() == 0))) return std::string("Unknown context");
    auto recentMessages = messages->slice(-3);
    return recentMessages->map([=](auto m) mutable
    {
        return m->content->text;
    }
    )->filter(Boolean)->join(std::string(" -> "));
};


string extractAction(string text)
{
    auto actionPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("trying to (.+?)(?:\.|,|$)"))), (new RegExp(std::string("attempted to (.+?)(?:\.|,|$)"))), (new RegExp(std::string("executed (.+?)(?:\.|,|$)"))), (new RegExp(std::string("ran (.+?)(?:\.|,|$)"))), (new RegExp(std::string("performed (.+?)(?:\.|,|$)"))) };
    for (auto& pattern : actionPatterns)
    {
        auto match = text->match(pattern);
        if (match) return (*const_(match))[1]->trim();
    }
    return std::string("performed action");
};


string extractError(string text)
{
    auto errorMatch = text->match((new RegExp(std::string("error:?\s*(.+?)(?:\.|$)"))));
    if (errorMatch) return (*const_(errorMatch))[1]->trim();
    auto failedMatch = text->match((new RegExp(std::string("failed:?\s*(.+?)(?:\.|$)"))));
    if (failedMatch) return (*const_(failedMatch))[1]->trim();
    return std::string("encountered error");
};


string extractDiscovery(string text)
{
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("found (?:that )?(.+?)(?:\.|,|$)"))), (new RegExp(std::string("discovered (?:that )?(.+?)(?:\.|,|$)"))), (new RegExp(std::string("realized (?:that )?(.+?)(?:\.|,|$)"))), (new RegExp(std::string("noticed (?:that )?(.+?)(?:\.|,|$)"))) };
    for (auto& pattern : patterns)
    {
        auto match = text->match(pattern);
        if (match) return (*const_(match))[1]->trim();
    }
    return std::string("made a discovery");
};


string extractLearning(string text, string type)
{
    auto learningMatch = text->match((new RegExp(std::string("(?:learned|learning|lesson):?\s*(.+?)(?:\.|$)"))));
    if (learningMatch) return (*const_(learningMatch))[1]->trim();
    static switch_type __switch14928_15422 = {
        { any(std::string("correction")), 1 },
        { any(std::string("discovery")), 2 },
        { any(std::string("success")), 3 }
    };
    switch (__switch14928_15422[type])
    {
    case 1:
        return std::string("Corrected approach works better than initial attempt");
    case 2:
        auto discovery = extractDiscovery(text);
        return (discovery != std::string("made a discovery")) ? any(discovery) : any(std::string("New capability or information discovered"));
    case 3:
        return std::string("This approach successfully completes the task");
    default:
        return std::string("Experience recorded for future reference");
    }
};


string extractHypothesis(string text)
{
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("i (?:think|believe) (?:that )?(.+?)(?:\.|$)"))), (new RegExp(std::string("hypothesis:?\s*(.+?)(?:\.|$)"))), (new RegExp(std::string("theory:?\s*(.+?)(?:\.|$)"))) };
    for (auto& pattern : patterns)
    {
        auto match = text->match(pattern);
        if (match) return (*const_(match))[1]->trim();
    }
    return std::string("formed hypothesis");
};


string detectDomain(string text)
{
    auto domains = object{
        object::pair{std::string("shell"), array<string>{ std::string("command"), std::string("terminal"), std::string("bash"), std::string("shell"), std::string("execute"), std::string("script") }}, 
        object::pair{std::string("coding"), array<string>{ std::string("code"), std::string("function"), std::string("variable"), std::string("syntax"), std::string("programming"), std::string("debug") }}, 
        object::pair{std::string("system"), array<string>{ std::string("file"), std::string("directory"), std::string("process"), std::string("memory"), std::string("cpu"), std::string("system") }}, 
        object::pair{std::string("network"), array<string>{ std::string("http"), std::string("api"), std::string("request"), std::string("response"), std::string("url"), std::string("network") }}, 
        object::pair{std::string("data"), array<string>{ std::string("json"), std::string("csv"), std::string("database"), std::string("query"), std::string("data") }}
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
    object::pair{std::string("description"), std::string("Evaluates agent actions and outcomes to record significant experiences and learnings")}, 
    object::pair{std::string("alwaysRun"), true}, 
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
