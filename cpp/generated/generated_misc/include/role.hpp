#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_ROLE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_ROLE_H
#include "core.h"
#include "../types/role-tags.h"

class Role;

class Role : public object, public std::enable_shared_from_this<Role> {
public:
    using std::enable_shared_from_this<Role>::shared_from_this;
    std::shared_ptr<Snowflake> id;

    string name;

    double color;

    boolean hoist;

    any icon;

    any unicode_emoji;

    double position;

    string permissions;

    boolean managed;

    boolean mentionable;

    RoleTags tags;

    double flags;

    Role(object opts);
};

#endif
