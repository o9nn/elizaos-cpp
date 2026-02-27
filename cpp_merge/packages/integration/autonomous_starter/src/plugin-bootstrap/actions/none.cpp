#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/actions/none.h"

std::shared_ptr<Action> noneAction = as<std::shared_ptr<Action>>(object{
    object::pair{std:("name"), std:("NONE")}, 
    object::pair{std:("similes"), array<string>{ std:("NO_ACTION"), std:("NO_RESPONSE"), std:("NO_REACTION") }}, 
    object::pair{std:("validate"), [=](auto _runtime, auto _message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("description"), std:("Respond but perform no additional action. This is the default if the agent is speaking and not doing anything additional.")}, 
    object::pair{std:("handler"), [=](auto _runtime, auto _message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Hey whats up")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("oh hey")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("did u see some faster whisper just came out")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("yeah but its a pain to get into node.js")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("the things that were funny 6 months ago are very cringe now")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("lol true")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("too real haha")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("gotta run")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Okay, ttyl")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), string_empty}, 
            object::pair{std:("actions"), array<string>{ std:("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("heyyyyyy")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("whats up long time no see")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("chillin man. playing lots of fortnite. what about you")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("u think aliens are real")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("ya obviously")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("drop a joke on me")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("why dont scientists trust atoms cuz they make up everything lmao")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("haha good one")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("hows the weather where ur at")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("beautiful all week")}, 
            object::pair{std:("actions"), array<string>{ std:("NONE") }}
        }}
    } } })}
});

void Main(void)
{
}

MAIN
