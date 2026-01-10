#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_REACTION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_REACTION_H
#include "core.h"
#include "../types/reaction-count-details-object.h"
#include "./emoji.h"

class Reaction;

class Reaction : public object, public std::enable_shared_from_this<Reaction> {
public:
    using std::enable_shared_from_this<Reaction>::shared_from_this;
    double count;

    ReactionCountDetailsObject count_details;

    boolean me;

    boolean me_burst;

    std::shared_ptr<Emoji> emoji;

    array<string> burst_colors;

    Reaction(object opts);
};

#endif
