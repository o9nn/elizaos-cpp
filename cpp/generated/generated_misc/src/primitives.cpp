#include "eliza/packages/core/src/types/primitives.h"

UUID asUUID(std::string id)
{
    if (OR((!id), (!(new RegExp(std::string("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")))->test(id)))) {
        throw std::any(std::make_shared<Error>(std::string("Invalid UUID format: ") + id + string_empty));
    }
    return as<UUID>(id);
};



void Main(void)
{
    string_empty + std::string + std::string("-") + std::string + std::string("-") + std::string + std::string("-") + std::string + std::string("-") + std::string + string_empty;
}

MAIN
