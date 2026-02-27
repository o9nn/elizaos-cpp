#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/hooks/use-admin-users.h"

std::function<object(boolean, double)> useAdminUsers = [=](auto showSuspended = false, auto limit = 50) mutable
{
    return usePagination(utils::assign(object{
        object::pair{std:("endpoint"), std:("/api/admin/users")}, 
        object::pair{std:("limit"), std:("limit")}, 
        object::pair{std:("itemsPropertyName"), std:("users")}, 
        object::pair{std:("sortBy"), std:("createdAt")}, 
        object::pair{std:("sortOrder"), std:("desc")}, 
        object::pair{std:("useUrlState"), true}
    }, (AND((showSuspended), (object{
        object::pair{std:("suspended"), std:("true")}
    })))));
};

void Main(void)
{
}

MAIN
