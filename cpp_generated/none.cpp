#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/actions/none.h"

std::shared_ptr<Action> noneAction = as<std::shared_ptr<Action>>(object{
    object::pair{std::string("name"), std::string("NONE")}, 
    object::pair{std::string("similes"), array<string>{ std::string("NO_ACTION"), std::string("NO_RESPONSE"), std::string("NO_REACTION") }}, 
    object::pair{std::string("validate"), [=](auto _runtime, auto _message) mutable
    {
        return true;
    }
    }, 
    object::pair{std::string("description"), std::string("Respond but perform no additional action. This is the default if the agent is speaking and not doing anything additional.")}, 
    object::pair{std::string("handler"), [=](auto _runtime, auto _message) mutable
    {
        return true;
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Hey whats up")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("oh hey")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("did u see some faster whisper just came out")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("yeah but its a pain to get into node.js")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("the things that were funny 6 months ago are very cringe now")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("lol true")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("too real haha")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("gotta run")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Okay, ttyl")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), string_empty}, 
            object::pair{std::string("actions"), array<string>{ std::string("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("heyyyyyy")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("whats up long time no see")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("chillin man. playing lots of fortnite. what about you")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("u think aliens are real")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("ya obviously")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("drop a joke on me")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("why dont scientists trust atoms cuz they make up everything lmao")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("haha good one")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("hows the weather where ur at")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("beautiful all week")}, 
            object::pair{std::string("actions"), array<string>{ std::string("NONE") }}
        }}
    } } })}
});

void Main(void)
{
}

MAIN
