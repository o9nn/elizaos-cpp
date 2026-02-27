#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_CLASSES_USER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_CLASSES_USER_H
#include "core.hpp"

class User;

class User : public object, public std::enable_shared_from_this<User> {
public:
    using std::enable_shared_from_this<User>::shared_from_this;
    std::shared_ptr<Snowflake> id;

    string username;

    string discriminator;

    any global_name;

    any avatar;

    boolean bot;

    boolean system;

    boolean mfa_enabled;

    any banner;

    any accent_color;

    string locale;

    boolean verified;

    any email;

    double flags;

    double premium_type;

    double public_flags;

    any avatar_decoration;

    User(object opts);
};

#endif
