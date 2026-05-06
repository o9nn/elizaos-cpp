#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/types/primitives.h"

UUID asUUID(string id)
{
    if (OR((!id), (!(new RegExp(std::string("^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")))->test(id)))) {
        throw any(std::make_shared<Error>(std::string("Invalid UUID format: ") + id + string_empty));
    }
    return as<UUID>(id);
};



void Main(void)
{
    string_empty + string + std::string("-") + string + std::string("-") + string + std::string("-") + string + std::string("-") + string + string_empty;
}

MAIN
