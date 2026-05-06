#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analytics/segmentation.h"

std::shared_ptr<Promise<array<std::shared_ptr<UserSegment>>>> UserSegmentation::generateSegments()
{
    return array<object>{ object{
        object::pair{std::string("id"), std::string("active")}, 
        object::pair{std::string("name"), std::string("Active Users")}, 
        object::pair{std::string("characteristics"), object{
            object::pair{std::string("activity"), std::string("high")}
        }}, 
        object::pair{std::string("size"), 100}
    } };
}

any prisma = std::make_shared<PrismaClient>();

void Main(void)
{
}

MAIN
