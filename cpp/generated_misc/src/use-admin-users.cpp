#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/hooks/use-admin-users.h"

std::function<object(boolean, double)> useAdminUsers = [=](auto showSuspended = false, auto limit = 50) mutable
{
    return usePagination(utils::assign(object{
        object::pair{std::string("endpoint"), std::string("/api/admin/users")}, 
        object::pair{std::string("limit"), std::string("limit")}, 
        object::pair{std::string("itemsPropertyName"), std::string("users")}, 
        object::pair{std::string("sortBy"), std::string("createdAt")}, 
        object::pair{std::string("sortOrder"), std::string("desc")}, 
        object::pair{std::string("useUrlState"), true}
    }, (AND((showSuspended), (object{
        object::pair{std::string("suspended"), std::string("true")}
    })))));
};

void Main(void)
{
}

MAIN
