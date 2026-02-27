#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/features/dm/use-dm-slice.h"

std::function<object()> useDmSlice = [=]() mutable
{
    shared dispatch = useAppDispatch();
    auto useDms = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["dm"]->dms;
        }
        );
    };
    auto useSelectedDms = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["dm"]->selectedDms;
        }
        );
    };
    auto useIsLoading = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["dm"]->isLoading;
        }
        );
    };
    auto usePreFilterUserId = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["dm"]->preFilterUserId;
        }
        );
    };
    auto usePreFilterUsers = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["dm"]->preFilterUsers;
        }
        );
    };
    auto state = object{
        object::pair{std:("dms"), useDms}, 
        object::pair{std:("selectedDms"), useSelectedDms}, 
        object::pair{std:("isLoading"), useIsLoading}, 
        object::pair{std:("preFilterUserId"), usePreFilterUserId}, 
        object::pair{std:("preFilterUsers"), usePreFilterUsers}
    };
    auto setIsLoading = [=](auto value) mutable
    {
        dispatch(setIsLoadingAction(value));
    };
    auto setDms = [=](auto dms) mutable
    {
        dispatch(setDmsAction(dms));
    };
    auto resetDm = [=]() mutable
    {
        dispatch(resetDmAction());
    };
    auto setPreFilterUserId = [=](P0 userId) mutable
    {
        dispatch(setPreFilterUserIdAction(userId));
    };
    auto getDms = [=]() mutable
    {
        dispatch(getDmsAction());
    };
    auto setSelectedDms = [=](auto dmIds) mutable
    {
        dispatch(mutateSelectedDmsAction(dmIds));
    };
    return object{
        object::pair{std:("state"), std:("state")}, 
        object::pair{std:("setIsLoading"), std:("setIsLoading")}, 
        object::pair{std:("setDms"), std:("setDms")}, 
        object::pair{std:("resetDm"), std:("resetDm")}, 
        object::pair{std:("setPreFilterUserId"), std:("setPreFilterUserId")}, 
        object::pair{std:("getDms"), std:("getDms")}, 
        object::pair{std:("setSelectedDms"), std:("setSelectedDms")}
    };
};

void Main(void)
{
}

MAIN
