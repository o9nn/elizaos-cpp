#include "eliza/packages/cli/src/utils/registry/schema.h"

std::any registrySchema = z->record(z->std::string(), z->std::string());

void Main(void)
{
}

MAIN
