#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/features/purge/use-purge-slice.h"

std::function<object()> usePurgeSlice = [=]() mutable
{
    shared dispatch = useAppDispatch();
    auto useIsLoading = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["purge"]->isLoading;
        }
        );
    };
    auto usePurgeChannel = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["purge"]->purgeChannel;
        }
        );
    };
    auto state = object{
        object::pair{std::string("isLoading"), useIsLoading}, 
        object::pair{std::string("purgeChannel"), usePurgeChannel}
    };
    auto setIsLoading = [=](auto value) mutable
    {
        dispatch(setIsLoadingAction(value));
    };
    shared purge = [=](auto channels) mutable
    {
        dispatch(purgeAction(channels));
    };
    return object{
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("setIsLoading"), std::string("setIsLoading")}, 
        object::pair{std::string("purge"), std::string("purge")}
    };
};

void Main(void)
{
}

MAIN
