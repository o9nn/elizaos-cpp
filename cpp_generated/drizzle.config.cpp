#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/drizzle.config.h"

void Main(void)
{
    config(object{
        object::pair{std::string("path"), std::string("../../.env")}
    });
}

MAIN
