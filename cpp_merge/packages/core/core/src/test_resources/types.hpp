#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_TEST_RESOURCES_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_TEST_RESOURCES_TYPES_H
#include "core.hpp"

class User;

class User : public object, public std::enable_shared_from_this<User> {
public:
    using std::enable_shared_from_this<User>::shared_from_this;
    string id;

    string email;

    string phone;

    string role;
};

#endif
