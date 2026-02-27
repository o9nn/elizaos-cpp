#include "prisma.hpp"

object globalForPrisma = as<object>(as<any>(global));
any prisma = OR((globalForPrisma["prisma"]), (std::make_shared<PrismaClient>()));

void Main(void)
{
    if (process->env->NODE_ENV != std:("production")) globalForPrisma["prisma"] = prisma;
}

MAIN
