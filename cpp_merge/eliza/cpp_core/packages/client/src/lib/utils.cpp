#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void cn() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return twMerge(clsx(inputs));

}

std: characterNameToUrl(const std:& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return name.replace(/\s+/g, "-");

}

std: urlToCharacterName(const std:& urlName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return urlName.replace(/-+/g, " ");

}

UUID randomUUID() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return URL.createObjectURL(std::make_unique<Blob>()).split("/").pop();

}

UUID getEntityId() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto USER_ID_KEY = "elizaos-client-user-id";
    const auto existingUserId = localStorage.getItem(USER_ID_KEY);

    if (existingUserId) {
        return existingUserId;
    }

    const auto newUserId = randomUUID();
    localStorage.setItem(USER_ID_KEY, newUserId);

    return newUserId;

}

} // namespace elizaos
