#include "utils.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void cn() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return twMerge(clsx(inputs));

}

std::string getRandomAvatar() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto randomIndex = Math.floor(((double)rand() / RAND_MAX) * PREDEFINED_AVATARS.size());
    return PREDEFINED_AVATARS[randomIndex];

}

} // namespace elizaos
