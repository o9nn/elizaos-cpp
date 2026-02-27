#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/prisma.h"

object globalForPrisma = as<object>(as<any>(global));
any prisma = OR((globalForPrisma["prisma"]), (std::make_shared<PrismaClient>()));

void Main(void)
{
    if (process->env->NODE_ENV != std::string("production")) globalForPrisma["prisma"] = prisma;
}

MAIN
