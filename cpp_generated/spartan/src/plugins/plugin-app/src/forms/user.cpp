#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-app/src/forms/user.h"

void userreg_create_addfields(any form, any values)
{
    form["addField"](std::string("email"), std::string("email"), values["email"], std::string("Email Address used for registration"));
};


