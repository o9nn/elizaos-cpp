#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/examples/examplePlugin.h"

std::shared_ptr<Action> exampleAction = object{
    object::pair{std::string("name"), std::string("EXAMPLE_ACTION")}, 
    object::pair{std::string("similes"), array<string>{ std::string("example action"), std::string("test action"), std::string("demo action") }}, 
    object::pair{std::string("description"), std::string("A simple example action that demonstrates dynamic plugin loading")}, 
    object::pair{std::string("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("User")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Run the example action")}, 
            object::pair{std::string("actions"), array<string>{ std::string("EXAMPLE_ACTION") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("Assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll run the example action for you.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("EXAMPLE_ACTION") }}
        }}
    } } }}, 
    , 
};
std::shared_ptr<Provider> exampleProvider = object{
    object::pair{std::string("name"), std::string("exampleProvider")}, 
    object::pair{std::string("description"), std::string("Provides example data from a dynamically loaded plugin")}, 
};
std::shared_ptr<Plugin> examplePlugin = object{
    object::pair{std::string("name"), std::string("example-plugin")}, 
    object::pair{std::string("description"), std::string("An example plugin that demonstrates dynamic loading and unloading")}, 
    object::pair{std::string("actions"), array<any>{ exampleAction }}, 
    object::pair{std::string("providers"), array<any>{ exampleProvider }}, 
    object::pair{std::string("init"), [=](auto config, auto runtime) mutable
    {
        console->log(std::string("[ExamplePlugin] Initializing with config:"), config);
        auto requiredVars = array<string>{ std::string("EXAMPLE_API_KEY") };
        auto missingVars = requiredVars->filter([=](auto v) mutable
        {
            return AND((!const_(process->env)[v]), (!runtime->getSetting(v)));
        }
        );
        if (missingVars->get_length() > 0) {
            console->warn(std::string("[ExamplePlugin] Missing environment variables: ") + missingVars->join(std::string(", ")) + string_empty);
        }
        console->log(std::string("[ExamplePlugin] Initialization complete"));
    }
    }
};

void Main(void)
{
}

MAIN
