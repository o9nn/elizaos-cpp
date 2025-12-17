#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void cn(const std::vector<ClassValue>& ...inputs) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return twMerge(clsx(inputs));

}

std::string getRandomAvatar() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto randomIndex = Math.floor(Math.random() * PREDEFINED_AVATARS.length);
    return PREDEFINED_AVATARS[randomIndex];

}

} // namespace elizaos
