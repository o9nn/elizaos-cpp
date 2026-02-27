#include "uuid.hpp"

any validateUuid(any value)
{
    return coreValidateUuid(value);
};


any uuidSchema = as<std::shared_ptr<z::ZodType<std::shared_ptr<UUID>>>>(z->string()->uuid());

void Main(void)
{
}

MAIN
