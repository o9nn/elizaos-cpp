#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/summarize/mutations.h"

std::shared_ptr<Promise<void>> storeDailySummary(string username, string date, string summary, std::shared_ptr<IntervalType> intervalType)
{
    auto id = string_empty + username + std::string("_") + intervalType + std::string("_") + date + string_empty;
    std::async([=]() { db->insert(userSummaries)->values(object{
        object::pair{std::string("id"), std::string("id")}, 
        object::pair{std::string("username"), std::string("username")}, 
        object::pair{std::string("date"), std::string("date")}, 
        object::pair{std::string("summary"), std::string("summary")}, 
        object::pair{std::string("intervalType"), std::string("intervalType")}, 
        object::pair{std::string("lastUpdated"), ((std::make_shared<UTCDate>()))->toISOString()}
    })->onConflictDoUpdate(object{
        object::pair{std::string("target"), userSummaries->id}, 
        object::pair{std::string("set"), object{
            object::pair{std::string("summary"), std::string("summary")}, 
            object::pair{std::string("lastUpdated"), ((std::make_shared<UTCDate>()))->toISOString()}
        }}
    }); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> storeRepoSummary(string repoId, string date, string summary, std::shared_ptr<IntervalType> intervalType)
{
    auto id = string_empty + repoId + std::string("_") + intervalType + std::string("_") + date + string_empty;
    std::async([=]() { db->insert(repoSummaries)->values(object{
        object::pair{std::string("id"), std::string("id")}, 
        object::pair{std::string("repoId"), std::string("repoId")}, 
        object::pair{std::string("date"), std::string("date")}, 
        object::pair{std::string("summary"), std::string("summary")}, 
        object::pair{std::string("intervalType"), std::string("intervalType")}, 
        object::pair{std::string("lastUpdated"), ((std::make_shared<UTCDate>()))->toISOString()}
    })->onConflictDoUpdate(object{
        object::pair{std::string("target"), repoSummaries->id}, 
        object::pair{std::string("set"), object{
            object::pair{std::string("summary"), std::string("summary")}, 
            object::pair{std::string("lastUpdated"), ((std::make_shared<UTCDate>()))->toISOString()}
        }}
    }); });
    return std::shared_ptr<Promise<void>>();
};


