#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/features/user/use-user-slice.h"

std::function<object()> useUserSlice = [=]() mutable
{
    shared dispatch = useAppDispatch();
    auto useCurrentUser = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["user"]->currentUser;
        }
        );
    };
    auto useToken = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["user"]->token;
        }
        );
    };
    auto useIsLoading = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["user"]->isLoading;
        }
        );
    };
    auto state = object{
        object::pair{std::string("currentUser"), useCurrentUser}, 
        object::pair{std::string("token"), useToken}, 
        object::pair{std::string("isLoading"), useIsLoading}
    };
    auto setIsLoading = [=](auto value) mutable
    {
        dispatch(setIsLoadingAction(value));
    };
    auto setToken = [=](P0 value) mutable
    {
        dispatch(setTokenAction(value));
    };
    auto setCurrentUser = [=](auto value) mutable
    {
        dispatch(setCurrentUserAction(value));
    };
    auto getUserData = [=]() mutable
    {
        dispatch(getUserDataAction());
    };
    auto getUserDataManaully = [=](auto token) mutable
    {
        return dispatch(getUserDataManaullyAction(token));
    };
    return object{
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("setIsLoading"), std::string("setIsLoading")}, 
        object::pair{std::string("setToken"), std::string("setToken")}, 
        object::pair{std::string("setCurrentUser"), std::string("setCurrentUser")}, 
        object::pair{std::string("getUserData"), std::string("getUserData")}, 
        object::pair{std::string("getUserDataManaully"), std::string("getUserDataManaully")}
    };
};

void Main(void)
{
}

MAIN
