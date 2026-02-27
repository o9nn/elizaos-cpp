#include "user.hpp"

void userreg_create_addfields(any form, any values)
{
    form["addField"](std:("email"), std:("email"), values["email"], std:("Email Address used for registration"));
};


