#include "uuid.hpp"
#include <string>

UUID asUUID(string id)
{
    auto validUuid = validateUuid(id);
    if (!validUuid) {
        throw any(std::make_shared<Error>(std::string("Invalid UUID format: ") + id + string_empty));
    }
    return as<UUID>(id->toLowerCase());
};


UUID generateUuidFromString(string input)
{
    return stringToUuid(input);
};


