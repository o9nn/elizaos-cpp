#include "plugin-specification/core-plugin-v2/src/uuid.h"

std::any validateUuid(std::any value)
{
    return coreValidateUuid(value);
};


std::any uuidSchema = as<std::shared_ptr<z::ZodType<std::shared_ptr<UUID>>>>(z->std::string()->uuid());

void Main(void)
{
}

MAIN
