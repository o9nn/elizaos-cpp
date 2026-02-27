#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/specs/v2/uuid.h"

any validateUuid(any value)
{
    return coreValidateUuid(value);
};


any uuidSchema = as<std::shared_ptr<z::ZodType<std::shared_ptr<UUID>>>>(z->string()->uuid());

void Main(void)
{
}

MAIN
