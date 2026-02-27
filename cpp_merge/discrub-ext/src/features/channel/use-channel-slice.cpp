#include "use-channel-slice.h"

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
        object::pair{std:("channels"), useChannels}, 
        object::pair{std:("selectedChannel"), useSelectedChannel}, 
        object::pair{std:("isLoading"), useIsLoading}, 
        object::pair{std:("selectedExportChannels"), useSelectedExportChannels}
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
        object::pair{std:("state"), std:("state")}, 
        object::pair{std:("setIsLoading"), std:("setIsLoading")}, 
        object::pair{std:("setChannels"), std:("setChannels")}, 
        object::pair{std:("setChannel"), std:("setChannel")}, 
        object::pair{std:("resetChannel"), std:("resetChannel")}, 
        object::pair{std:("setSelectedExportChannels"), std:("setSelectedExportChannels")}, 
        object::pair{std:("getChannels"), std:("getChannels")}, 
        object::pair{std:("changeChannel"), std:("changeChannel")}, 
        object::pair{std:("loadChannel"), std:("loadChannel")}
    };
};

void Main(void)
{
}

MAIN
