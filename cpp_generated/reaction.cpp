#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/classes/reaction.h"

Reaction::Reaction(object opts) {
    this->count = opts["count"];
    this->count_details = opts["count_details"];
    this->me = opts["me"];
    this->me_burst = opts["me_burst"];
    this->emoji = opts["emoji"];
    this->burst_colors = opts["burst_colors"];
}

