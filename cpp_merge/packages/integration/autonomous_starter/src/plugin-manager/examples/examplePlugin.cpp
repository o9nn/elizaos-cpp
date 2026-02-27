#include "examplePlugin.hpp"

std::shared_ptr<Action> exampleAction = object{
    object::pair{std:("name"), std:("EXAMPLE_ACTION")}, 
    object::pair{std:("similes"), array<string>{ std:("example action"), std:("test action"), std:("demo action") }}, 
    object::pair{std:("description"), std:("A simple example action that demonstrates dynamic plugin loading")}, 
    object::pair{std:("examples"), array<array<object>>{ array<object>{ object{
        object::pair{std:("name"), std:("User")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Run the example action")}, 
            object::pair{std:("actions"), array<string>{ std:("EXAMPLE_ACTION") }}
        }}
    }, object{
        object::pair{std:("name"), std:("Assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll run the example action for you.")}, 
            object::pair{std:("actions"), array<string>{ std:("EXAMPLE_ACTION") }}
        }}
    } } }}, 
    , 
};
std::shared_ptr<Provider> exampleProvider = object{
    object::pair{std:("name"), std:("exampleProvider")}, 
    object::pair{std:("description"), std:("Provides example data from a dynamically loaded plugin")}, 
};
std::shared_ptr<Plugin> examplePlugin = object{
    object::pair{std:("name"), std:("example-plugin")}, 
    object::pair{std:("description"), std:("An example plugin that demonstrates dynamic loading and unloading")}, 
    object::pair{std:("actions"), array<any>{ exampleAction }}, 
    object::pair{std:("providers"), array<any>{ exampleProvider }}, 
    object::pair{std:("init"), [=](auto config, auto runtime) mutable
    {
        console->log(std:("[ExamplePlugin] Initializing with config:"), config);
        auto requiredVars = array<string>{ std:("EXAMPLE_API_KEY") };
        auto missingVars = requiredVars->filter([=](auto v) mutable
        {
            return AND((!const_(process->env)[v]), (!runtime->getSetting(v)));
        }
        );
        if (missingVars->get_length() > 0) {
            console->warn(std:("[ExamplePlugin] Missing environment variables: ") + missingVars->join(std:(", ")) + string_empty);
        }
        console->log(std:("[ExamplePlugin] Initialization complete"));
    }
    }
};

void Main(void)
{
}

MAIN
