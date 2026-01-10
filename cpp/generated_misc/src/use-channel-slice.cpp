#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/features/channel/use-channel-slice.h"

std::function<object()> useChannelSlice = [=]() mutable
{
    shared dispatch = useAppDispatch();
    auto useChannels = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["channel"]->channels;
        }
        );
    };
    auto useSelectedChannel = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["channel"]->selectedChannel;
        }
        );
    };
    auto useIsLoading = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["channel"]->isLoading;
        }
        );
    };
    auto useSelectedExportChannels = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["channel"]->selectedExportChannels;
        }
        );
    };
    auto state = object{
        object::pair{std::string("channels"), useChannels}, 
        object::pair{std::string("selectedChannel"), useSelectedChannel}, 
        object::pair{std::string("isLoading"), useIsLoading}, 
        object::pair{std::string("selectedExportChannels"), useSelectedExportChannels}
    };
    auto setIsLoading = [=](auto value) mutable
    {
        dispatch(setIsLoadingAction(value));
    };
    auto setChannels = [=](auto channels) mutable
    {
        dispatch(setChannelsAction(channels));
    };
    auto setChannel = [=](auto channelId) mutable
    {
        dispatch(setChannelAction(channelId));
    };
    auto resetChannel = [=]() mutable
    {
        dispatch(resetChannelAction());
    };
    auto setSelectedExportChannels = [=](auto channelIds) mutable
    {
        dispatch(setSelectedExportChannelsAction(channelIds));
    };
    auto getChannels = [=](auto guildId) mutable
    {
        dispatch(getChannelsAction(guildId));
    };
    auto changeChannel = [=](auto channelId) mutable
    {
        dispatch(changeChannelAction(channelId));
    };
    auto loadChannel = [=](auto channelId) mutable
    {
        dispatch(loadChannelAction(channelId));
    };
    return object{
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("setIsLoading"), std::string("setIsLoading")}, 
        object::pair{std::string("setChannels"), std::string("setChannels")}, 
        object::pair{std::string("setChannel"), std::string("setChannel")}, 
        object::pair{std::string("resetChannel"), std::string("resetChannel")}, 
        object::pair{std::string("setSelectedExportChannels"), std::string("setSelectedExportChannels")}, 
        object::pair{std::string("getChannels"), std::string("getChannels")}, 
        object::pair{std::string("changeChannel"), std::string("changeChannel")}, 
        object::pair{std::string("loadChannel"), std::string("loadChannel")}
    };
};

void Main(void)
{
}

MAIN
