#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> calculatorPlugin = object{
    object::pair{std::string("name"), std::string("calculator")}, 
    object::pair{std::string("description"), std::string("Basic arithmetic calculator plugin")}, 
    object::pair{std::string("actions"), array<any>{ calculateAction }}, 
    object::pair{std::string("evaluators"), array<any>{ calculateEvaluator }}, 
    object::pair{std::string("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
