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
        object::pair{std:("currentUser"), useCurrentUser}, 
        object::pair{std:("token"), useToken}, 
        object::pair{std:("isLoading"), useIsLoading}
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
        object::pair{std:("state"), std:("state")}, 
        object::pair{std:("setIsLoading"), std:("setIsLoading")}, 
        object::pair{std:("setToken"), std:("setToken")}, 
        object::pair{std:("setCurrentUser"), std:("setCurrentUser")}, 
        object::pair{std:("getUserData"), std:("getUserData")}, 
        object::pair{std:("getUserDataManaully"), std:("getUserDataManaully")}
    };
};

void Main(void)
{
}

MAIN
