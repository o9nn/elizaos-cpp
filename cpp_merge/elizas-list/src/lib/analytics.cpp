#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/analytics.h"

void Analytics::trackProjectView(string projectId, string userId)
{
    std::async([=]() { prisma["projectView"]["create"](object{
        object::pair{std:("data"), object{
            object::pair{std:("projectId"), std:("projectId")}, 
            object::pair{std:("userId"), std:("userId")}, 
            object::pair{std:("timestamp"), std::make_shared<Date>()}, 
            object::pair{std:("userAgent"), OR((headers()->get(std:("user-agent"))), (std:("unknown")))}, 
            object::pair{std:("ipAddress"), OR((headers()->get(std:("x-forwarded-for"))), (std:("unknown")))}
        }}
    }); });
}

void Analytics::trackRelatedProjectClick(string sourceProjectId, string targetProjectId, string userId)
{
    std::async([=]() { prisma["projectInteraction"]["create"](object{
        object::pair{std:("data"), object{
            object::pair{std:("sourceProjectId"), std:("sourceProjectId")}, 
            object::pair{std:("targetProjectId"), std:("targetProjectId")}, 
            object::pair{std:("userId"), std:("userId")}, 
            object::pair{std:("type"), std:("RELATED_CLICK")}, 
            object::pair{std:("timestamp"), std::make_shared<Date>()}
        }}
    }); });
}

