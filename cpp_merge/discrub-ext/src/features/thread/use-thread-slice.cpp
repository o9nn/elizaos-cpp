#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/features/thread/use-thread-slice.h"

std::function<object()> useThreadSlice = [=]() mutable
{
    shared dispatch = useAppDispatch();
    auto useThreads = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["thread"]->threads;
        }
        );
    };
    auto state = object{
        object::pair{std:("threads"), useThreads}
    };
    auto setThreads = [=](auto value) mutable
    {
        dispatch(setThreadsAction(value));
    };
    auto resetThreads = [=]() mutable
    {
        dispatch(resetThreadsAction());
    };
    auto getArchivedThreads = [=](auto channelId, auto knownThreads) mutable
    {
        auto threads = std::async([=]() { dispatch(getArchivedThreadsAction(object{
            object::pair{std:("channelId"), std:("channelId")}, 
            object::pair{std:("knownThreads"), std:("knownThreads")}
        })); });
        return threads;
    };
    auto getThreadsFromMessages = [=](auto messages, auto knownThreads) mutable
    {
        return getThreadsFromMessagesAction(object{
            object::pair{std:("messages"), std:("messages")}, 
            object::pair{std:("knownThreads"), std:("knownThreads")}
        });
    };
    auto unarchiveThread = template <typename RET>
    [=](auto threadId) mutable
    {
        auto thread = std::async([=]() { dispatch(unarchiveThreadAction(threadId)); });
        return thread;
    };
    auto liftThreadRestrictions = [=](auto channelId, auto noPermissionThreadIds) mutable
    {
        auto noPermissionIds = std::async([=]() { dispatch(liftThreadRestrictionsAction(object{
            object::pair{std:("channelId"), std:("channelId")}, 
            object::pair{std:("noPermissionThreadIds"), std:("noPermissionThreadIds")}
        })); });
        return noPermissionIds;
    };
    return object{
        object::pair{std:("state"), std:("state")}, 
        object::pair{std:("setThreads"), std:("setThreads")}, 
        object::pair{std:("resetThreads"), std:("resetThreads")}, 
        object::pair{std:("getArchivedThreads"), std:("getArchivedThreads")}, 
        object::pair{std:("getThreadsFromMessages"), std:("getThreadsFromMessages")}, 
        object::pair{std:("unarchiveThread"), std:("unarchiveThread")}, 
        object::pair{std:("liftThreadRestrictions"), std:("liftThreadRestrictions")}
    };
};

void Main(void)
{
}

MAIN
