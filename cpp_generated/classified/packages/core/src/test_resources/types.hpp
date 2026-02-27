#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TEST_RESOURCES_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TEST_RESOURCES_TYPES_H
#include "core.h"

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
