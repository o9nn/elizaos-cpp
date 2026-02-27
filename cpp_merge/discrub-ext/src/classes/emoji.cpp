#include "emoji.hpp"

Emoji::Emoji(object opts) {
    this->id = opts["id"];
    this->name = opts["name"];
    this->roles = opts["roles"];
    this->user = opts["user"];
    this->require_colons = opts["require_colons"];
    this->managed = opts["managed"];
    this->animated = opts["animated"];
    this->available = opts["available"];
}

