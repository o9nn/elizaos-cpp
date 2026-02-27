#include "base.hpp"

form::form() {
    this->fields = array<any>();
}

void form::addField(any type, any name, any value, any label, any validation, any options)
{
    this->fields["push"](object{
        object::pair{std:("type"), std:("type")}, 
        object::pair{std:("name"), std:("name")}, 
        object::pair{std:("value"), std:("value")}, 
        object::pair{std:("label"), std:("label")}, 
        object::pair{std:("validation"), std:("validation")}, 
        object::pair{std:("options"), std:("options")}
    });
}

