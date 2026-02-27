#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-plugin-starter/src/plugins/calculator/index.h"

std::shared_ptr<Plugin> calculatorPlugin = object{
    object::pair{std:("name"), std:("calculator")}, 
    object::pair{std:("description"), std:("Basic arithmetic calculator plugin")}, 
    object::pair{std:("actions"), array<any>{ calculateAction }}, 
    object::pair{std:("evaluators"), array<any>{ calculateEvaluator }}, 
    object::pair{std:("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
