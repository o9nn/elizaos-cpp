#include "user.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void userreg_create_addfields(auto form, auto values) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    form.addField("email", "email", values.email, "Email Address used for registration");

}

} // namespace elizaos
