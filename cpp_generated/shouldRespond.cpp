#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/providers/shouldRespond.h"

std::shared_ptr<Config> nameConfig = object{
    object::pair{std::string("dictionaries"), array<any>{ adjectives, names }}, 
    object::pair{std::string("separator"), string_empty}, 
    object::pair{std::string("length"), 2}, 
    object::pair{std::string("style"), std::string("capital")}
};
array<string> messageExamples = array<string>{ std::string("// {{name1}}: Hey {{agentName}}, can you help me with something\
// Response: RESPOND"), std::string("// {{name1}}: Hey {{agentName}}, can I ask you a question\
// {{agentName}}: Sure, what is it\
// {{name1}}: can you help me create a basic react module that demonstrates a counter\
// Response: RESPOND"), std::string("// {{name1}}: {{agentName}} can you tell me a story\
// {{name1}}: about a girl named {{characterName}}\
// {{agentName}}: Sure.\
// {{agentName}}: Once upon a time, in a quaint little village, there was a curious girl named {{characterName}}.\
// {{agentName}}: {{characterName}} was known for her adventurous spirit and her knack for finding beauty in the mundane.\
// {{name1}}: I'm loving it, keep going\
// Response: RESPOND"), std::string("// {{name1}}: okay, i want to test something. can you say marco?\
// {{agentName}}: marco\
// {{name1}}: great. okay, now do it again\
// Response: RESPOND"), std::string("// {{name1}}: what do you think about artificial intelligence?\
// Response: RESPOND"), std::string("// {{name1}}: I just saw a really great movie\
// {{name2}}: Oh? Which movie?\
// Response: IGNORE"), std::string("// {{name1}}: i need help\
// {{agentName}}: how can I help you?\
// {{name1}}: no. i need help from {{name2}}\
// Response: IGNORE"), std::string("// {{name1}}: {{name2}} can you answer a question for me?\
// Response: IGNORE"), std::string("// {{agentName}}: Oh, this is my favorite scene\
// {{name1}}: sick\
// {{name2}}: wait, why is it your favorite scene\
// Response: RESPOND"), std::string("// {{name1}}: {{agentName}} stop responding plz\
// Response: STOP"), std::string("// {{name1}}: stfu bot\
// Response: STOP"), std::string("// {{name1}}: {{agentName}} stfu plz\
// Response: STOP") };
std::shared_ptr<Provider> shouldRespondProvider = object{
    object::pair{std::string("name"), std::string("SHOULD_RESPOND")}, 
    object::pair{std::string("description"), std::string("Examples of when the agent should respond, ignore, or stop responding")}, 
    object::pair{std::string("position"), -1}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message) mutable
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
            return example->replace((new RegExp(std::string("{{name1}}"))), name1)->replace((new RegExp(std::string("{{name2}}"))), name2)->replace((new RegExp(std::string("{{agentName}}"))), agentName)->replace((new RegExp(std::string("{{characterName}}"))), characterName);
        }
        );
        auto text = addHeader(std::string("# RESPONSE EXAMPLES"), formattedExamples->join(std::string("\
\
")));
        return object{
            object::pair{std::string("text"), std::string("text")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
