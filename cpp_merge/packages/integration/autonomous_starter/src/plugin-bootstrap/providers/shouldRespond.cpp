#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/providers/shouldRespond.h"

std::shared_ptr<Config> nameConfig = object{
    object::pair{std:("dictionaries"), array<any>{ adjectives, names }}, 
    object::pair{std:("separator"), string_empty}, 
    object::pair{std:("length"), 2}, 
    object::pair{std:("style"), std:("capital")}
};
array<string> messageExamples = array<string>{ std:("// {{name1}}: Hey {{agentName}}, can you help me with something\
// Response: RESPOND"), std:("// {{name1}}: Hey {{agentName}}, can I ask you a question\
// {{agentName}}: Sure, what is it\
// {{name1}}: can you help me create a basic react module that demonstrates a counter\
// Response: RESPOND"), std:("// {{name1}}: {{agentName}} can you tell me a story\
// {{name1}}: about a girl named {{characterName}}\
// {{agentName}}: Sure.\
// {{agentName}}: Once upon a time, in a quaint little village, there was a curious girl named {{characterName}}.\
// {{agentName}}: {{characterName}} was known for her adventurous spirit and her knack for finding beauty in the mundane.\
// {{name1}}: I'm loving it, keep going\
// Response: RESPOND"), std:("// {{name1}}: okay, i want to test something. can you say marco?\
// {{agentName}}: marco\
// {{name1}}: great. okay, now do it again\
// Response: RESPOND"), std:("// {{name1}}: what do you think about artificial intelligence?\
// Response: RESPOND"), std:("// {{name1}}: I just saw a really great movie\
// {{name2}}: Oh? Which movie?\
// Response: IGNORE"), std:("// {{name1}}: i need help\
// {{agentName}}: how can I help you?\
// {{name1}}: no. i need help from {{name2}}\
// Response: IGNORE"), std:("// {{name1}}: {{name2}} can you answer a question for me?\
// Response: IGNORE"), std:("// {{agentName}}: Oh, this is my favorite scene\
// {{name1}}: sick\
// {{name2}}: wait, why is it your favorite scene\
// Response: RESPOND"), std:("// {{name1}}: {{agentName}} stop responding plz\
// Response: STOP"), std:("// {{name1}}: stfu bot\
// Response: STOP"), std:("// {{name1}}: {{agentName}} stfu plz\
// Response: STOP") };
std::shared_ptr<Provider> shouldRespondProvider = object{
    object::pair{std:("name"), std:("SHOULD_RESPOND")}, 
    object::pair{std:("description"), std:("Examples of when the agent should respond, ignore, or stop responding")}, 
    object::pair{std:("position"), -1}, 
    object::pair{std:("get"), [=](auto runtime, auto _message) mutable
    {
        shared agentName = runtime->character->name;
        shared name1 = uniqueNamesGenerator(nameConfig);
        shared name2 = uniqueNamesGenerator(nameConfig);
        shared characterName = uniqueNamesGenerator(nameConfig);
        auto shuffledExamples = (array<string>{ messageExamples })->sort([=]() mutable
        {
            return 0.5 - Math->random();
        }
        )->slice(0, 7);
        auto formattedExamples = shuffledExamples->map([=](auto example) mutable
        {
            return example->replace((new RegExp(std:("{{name1}}"))), name1)->replace((new RegExp(std:("{{name2}}"))), name2)->replace((new RegExp(std:("{{agentName}}"))), agentName)->replace((new RegExp(std:("{{characterName}}"))), characterName);
        }
        );
        auto text = addHeader(std:("# RESPONSE EXAMPLES"), formattedExamples->join(std:("\
\
")));
        return object{
            object::pair{std:("text"), std:("text")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
