#include "user.hpp"
#include <string>

void userreg_create_addfields(any form, any values)
{
    form["addField"](std::string("email"), std::string("email"), values["email"], std::string("Email Address used for registration"));
};


