#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_EMOJI_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_EMOJI_H
#include "core.h"
#include "./user.h"

class Emoji;

class Emoji : public object, public std::enable_shared_from_this<Emoji> {
public:
    using std::enable_shared_from_this<Emoji>::shared_from_this;
    any id;

    any name;

    array<std::shared_ptr<Snowflake>> roles;

    std::shared_ptr<User> user;

    boolean require_colons;

    boolean managed;

    boolean animated;

    boolean available;

    Emoji(object opts);
};

#endif
