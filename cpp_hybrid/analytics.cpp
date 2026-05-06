#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analytics.h"

void Analytics::trackProjectView(string projectId, string userId)
{
    std::async([=]() { prisma["projectView"]["create"](object{
        object::pair{std::string("data"), object{
            object::pair{std::string("projectId"), std::string("projectId")}, 
            object::pair{std::string("userId"), std::string("userId")}, 
            object::pair{std::string("timestamp"), std::make_shared<Date>()}, 
            object::pair{std::string("userAgent"), OR((headers()->get(std::string("user-agent"))), (std::string("unknown")))}, 
            object::pair{std::string("ipAddress"), OR((headers()->get(std::string("x-forwarded-for"))), (std::string("unknown")))}
        }}
    }); });
}

void Analytics::trackRelatedProjectClick(string sourceProjectId, string targetProjectId, string userId)
{
    std::async([=]() { prisma["projectInteraction"]["create"](object{
        object::pair{std::string("data"), object{
            object::pair{std::string("sourceProjectId"), std::string("sourceProjectId")}, 
            object::pair{std::string("targetProjectId"), std::string("targetProjectId")}, 
            object::pair{std::string("userId"), std::string("userId")}, 
            object::pair{std::string("type"), std::string("RELATED_CLICK")}, 
            object::pair{std::string("timestamp"), std::make_shared<Date>()}
        }}
    }); });
}

