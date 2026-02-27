#include "tests.hpp"
#include <string>

std::shared_ptr<TestSuite> PluginTestSuite = object{
    object::pair{std::string("name"), std::string("plugin_starter_test_suite")}, 
    object::pair{std::string("tests"), array<any>()}
};

void Main(void)
{
}

MAIN
