#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/summarize/mutations.h"

std::shared_ptr<Promise<void>> storeDailySummary(string username, string date, string summary, std::shared_ptr<IntervalType> intervalType)
{
    auto id = string_empty + username + std:("_") + intervalType + std:("_") + date + string_empty;
    std::async([=]() { db->insert(userSummaries)->values(object{
        object::pair{std:("id"), std:("id")}, 
        object::pair{std:("username"), std:("username")}, 
        object::pair{std:("date"), std:("date")}, 
        object::pair{std:("summary"), std:("summary")}, 
        object::pair{std:("intervalType"), std:("intervalType")}, 
        object::pair{std:("lastUpdated"), ((std::make_shared<UTCDate>()))->toISOString()}
    })->onConflictDoUpdate(object{
        object::pair{std:("target"), userSummaries->id}, 
        object::pair{std:("set"), object{
            object::pair{std:("summary"), std:("summary")}, 
            object::pair{std:("lastUpdated"), ((std::make_shared<UTCDate>()))->toISOString()}
        }}
    }); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> storeRepoSummary(string repoId, string date, string summary, std::shared_ptr<IntervalType> intervalType)
{
    auto id = string_empty + repoId + std:("_") + intervalType + std:("_") + date + string_empty;
    std::async([=]() { db->insert(repoSummaries)->values(object{
        object::pair{std:("id"), std:("id")}, 
        object::pair{std:("repoId"), std:("repoId")}, 
        object::pair{std:("date"), std:("date")}, 
        object::pair{std:("summary"), std:("summary")}, 
        object::pair{std:("intervalType"), std:("intervalType")}, 
        object::pair{std:("lastUpdated"), ((std::make_shared<UTCDate>()))->toISOString()}
    })->onConflictDoUpdate(object{
        object::pair{std:("target"), repoSummaries->id}, 
        object::pair{std:("set"), object{
            object::pair{std:("summary"), std:("summary")}, 
            object::pair{std:("lastUpdated"), ((std::make_shared<UTCDate>()))->toISOString()}
        }}
    }); });
    return std::shared_ptr<Promise<void>>();
};


