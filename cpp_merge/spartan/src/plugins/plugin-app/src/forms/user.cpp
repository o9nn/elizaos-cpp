#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-app/src/forms/user.h"

void userreg_create_addfields(any form, any values)
{
    form["addField"](std:("email"), std:("email"), values["email"], std:("Email Address used for registration"));
};


