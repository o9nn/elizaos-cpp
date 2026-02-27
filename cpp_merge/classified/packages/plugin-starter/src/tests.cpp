#include "tests.hpp"

std::shared_ptr<TestSuite> PluginTestSuite = object{
    object::pair{std:("name"), std:("plugin_starter_test_suite")}, 
    object::pair{std:("tests"), array<any>()}
};

void Main(void)
{
}

MAIN
