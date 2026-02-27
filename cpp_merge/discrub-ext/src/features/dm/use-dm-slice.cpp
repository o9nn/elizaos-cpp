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
        object::pair{std::string("dms"), useDms}, 
        object::pair{std::string("selectedDms"), useSelectedDms}, 
        object::pair{std::string("isLoading"), useIsLoading}, 
        object::pair{std::string("preFilterUserId"), usePreFilterUserId}, 
        object::pair{std::string("preFilterUsers"), usePreFilterUsers}
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
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("setIsLoading"), std::string("setIsLoading")}, 
        object::pair{std::string("setDms"), std::string("setDms")}, 
        object::pair{std::string("resetDm"), std::string("resetDm")}, 
        object::pair{std::string("setPreFilterUserId"), std::string("setPreFilterUserId")}, 
        object::pair{std::string("getDms"), std::string("getDms")}, 
        object::pair{std::string("setSelectedDms"), std::string("setSelectedDms")}
    };
};

void Main(void)
{
}

MAIN
