#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/classes/role.h"

Role::Role(object opts) {
    this->id = opts["id"];
    this->name = opts["name"];
    this->color = opts["color"];
    this->hoist = opts["hoist"];
    this->icon = opts["icon"];
    this->unicode_emoji = opts["unicode_emoji"];
    this->position = opts["position"];
    this->permissions = opts["permissions"];
    this->managed = opts["managed"];
    this->mentionable = opts["mentionable"];
    this->tags = opts["tags"];
    this->flags = opts["flags"];
}

