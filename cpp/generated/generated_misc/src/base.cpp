#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-app/src/forms/base.h"

form::form() {
    this->fields = array<any>();
}

void form::addField(std::any type, std::any name, std::any value, std::any label, std::any validation, std::any options)
{
    this->fields["push"](object{
        object::pair{std::string("type"), std::string("type")}, 
        object::pair{std::string("name"), std::string("name")}, 
        object::pair{std::string("value"), std::string("value")}, 
        object::pair{std::string("label"), std::string("label")}, 
        object::pair{std::string("validation"), std::string("validation")}, 
        object::pair{std::string("options"), std::string("options")}
    });
}

