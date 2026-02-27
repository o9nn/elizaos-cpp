#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/drizzle.config.h"

void Main(void)
{
    dotenv->config(object{
        object::pair{std::string("path"), std::string("../../.env")}
    });
}

MAIN
