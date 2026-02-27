#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/search-analytics.h"

void trackSearch(string query, string userId)
{
    std::async([=]() { prisma->searchQuery->create(object{
        object::pair{std:("data"), object{
            object::pair{std:("query"), std:("query")}, 
            object::pair{std:("userId"), std:("userId")}, 
            object::pair{std:("timestamp"), std::make_shared<Date>()}
        }}
    }); });
};


any getPopularSearches(double limit)
{
    auto searches = std::async([=]() { prisma->searchQuery->groupBy(object{
        object::pair{std:("by"), array<string>{ std:("query") }}, 
        object::pair{std:("_count"), object{
            object::pair{std:("query"), true}
        }}, 
        object::pair{std:("orderBy"), object{
            object::pair{std:("_count"), object{
                object::pair{std:("query"), std:("desc")}
            }}
        }}, 
        object::pair{std:("take"), limit}
    }); });
    return searches;
};


