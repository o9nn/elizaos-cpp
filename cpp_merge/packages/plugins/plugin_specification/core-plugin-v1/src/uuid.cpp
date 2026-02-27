#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/uuid.h"

UUID asUUID(string id)
{
    auto validUuid = validateUuid(id);
    if (!validUuid) {
        throw any(std::make_shared<Error>(std:("Invalid UUID format: ") + id + string_empty));
    }
    return as<UUID>(id->toLowerCase());
};


UUID generateUuidFromString(string input)
{
    return stringToUuid(input);
};


