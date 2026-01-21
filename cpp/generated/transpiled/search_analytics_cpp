#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/search-analytics.h"

void trackSearch(string query, string userId)
{
    std::async([=]() { prisma->searchQuery->create(object{
        object::pair{std::string("data"), object{
            object::pair{std::string("query"), std::string("query")}, 
            object::pair{std::string("userId"), std::string("userId")}, 
            object::pair{std::string("timestamp"), std::make_shared<Date>()}
        }}
    }); });
};


any getPopularSearches(double limit)
{
    auto searches = std::async([=]() { prisma->searchQuery->groupBy(object{
        object::pair{std::string("by"), array<string>{ std::string("query") }}, 
        object::pair{std::string("_count"), object{
            object::pair{std::string("query"), true}
        }}, 
        object::pair{std::string("orderBy"), object{
            object::pair{std::string("_count"), object{
                object::pair{std::string("query"), std::string("desc")}
            }}
        }}, 
        object::pair{std::string("take"), limit}
    }); });
    return searches;
};


