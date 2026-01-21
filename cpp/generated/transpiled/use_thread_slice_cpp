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
        object::pair{std::string("threads"), useThreads}
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
            object::pair{std::string("channelId"), std::string("channelId")}, 
            object::pair{std::string("knownThreads"), std::string("knownThreads")}
        })); });
        return threads;
    };
    auto getThreadsFromMessages = [=](auto messages, auto knownThreads) mutable
    {
        return getThreadsFromMessagesAction(object{
            object::pair{std::string("messages"), std::string("messages")}, 
            object::pair{std::string("knownThreads"), std::string("knownThreads")}
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
            object::pair{std::string("channelId"), std::string("channelId")}, 
            object::pair{std::string("noPermissionThreadIds"), std::string("noPermissionThreadIds")}
        })); });
        return noPermissionIds;
    };
    return object{
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("setThreads"), std::string("setThreads")}, 
        object::pair{std::string("resetThreads"), std::string("resetThreads")}, 
        object::pair{std::string("getArchivedThreads"), std::string("getArchivedThreads")}, 
        object::pair{std::string("getThreadsFromMessages"), std::string("getThreadsFromMessages")}, 
        object::pair{std::string("unarchiveThread"), std::string("unarchiveThread")}, 
        object::pair{std::string("liftThreadRestrictions"), std::string("liftThreadRestrictions")}
    };
};

void Main(void)
{
}

MAIN
