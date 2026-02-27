#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analytics/segmentation.h"

std::shared_ptr<Promise<array<std::shared_ptr<UserSegment>>>> UserSegmentation::generateSegments()
{
    return array<object>{ object{
        object::pair{std:("id"), std:("active")}, 
        object::pair{std:("name"), std:("Active Users")}, 
        object::pair{std:("characteristics"), object{
            object::pair{std:("activity"), std:("high")}
        }}, 
        object::pair{std:("size"), 100}
    } };
}

any prisma = std::make_shared<PrismaClient>();

void Main(void)
{
}

MAIN
