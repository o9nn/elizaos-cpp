#include "user.hpp"

User::User(object opts) {
    this->id = opts["id"];
    this->username = opts["username"];
    this->discriminator = opts["discriminator"];
    this->global_name = opts["global_name"];
    this->avatar = opts["avatar"];
    this->bot = opts["bot"];
    this->system = opts["system"];
    this->mfa_enabled = opts["mfa_enabled"];
    this->banner = opts["banner"];
    this->accent_color = opts["accent_color"];
    this->locale = opts["locale"];
    this->verified = opts["verified"];
    this->email = opts["email"];
    this->flags = opts["flags"];
    this->premium_type = opts["premium_type"];
    this->public_flags = opts["public_flags"];
    this->avatar_decoration = opts["avatar_decoration"];
}

