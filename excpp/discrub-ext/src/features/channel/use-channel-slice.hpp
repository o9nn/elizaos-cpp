#include "..app/hooks.hpp"
#include "..app/store.hpp"
#include "..classes/channel.hpp"
#include "channel-slice.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;

const useChannelSlice = () => {
  const dispatch = useAppDispatch();

  const useChannels = (): Channel[] =>
    useAppSelector((state: RootState) => state.channel.channels);

  const useSelectedChannel = (): Channel | Maybe =>
    useAppSelector((state: RootState) => state.channel.selectedChannel);

  const useIsLoading = (): boolean | Maybe =>
    useAppSelector((state: RootState) => state.channel.isLoading);

  const useSelectedExportChannels = (): Snowflake[] =>
    useAppSelector((state: RootState) => state.channel.selectedExportChannels);

  const state = {
    channels: useChannels,
    selectedChannel: useSelectedChannel,
    isLoading: useIsLoading,
    selectedExportChannels: useSelectedExportChannels,
  };

  const setIsLoading = (value: boolean): void => {
    dispatch(setIsLoadingAction(value));
  };
  const setChannels = (channels: Channel[]): void => {
    dispatch(setChannelsAction(channels));
  };
  const setChannel = (channelId: Snowflake): void => {
    dispatch(setChannelAction(channelId));
  };
  const resetChannel = (): void => {
    dispatch(resetChannelAction());
  };
  const setSelectedExportChannels = (channelIds: string[]): void => {
    dispatch(setSelectedExportChannelsAction(channelIds));
  };
  const getChannels = (guildId: Snowflake): void => {
    dispatch(getChannelsAction(guildId));
  };

  const changeChannel = (channelId: Snowflake | null): void => {
    dispatch(changeChannelAction(channelId));
  };

  const loadChannel = (channelId: Snowflake): void => {
    dispatch(loadChannelAction(channelId));
  };

  return {
    state,
    setIsLoading,
    setChannels,
    setChannel,
    resetChannel,
    setSelectedExportChannels,
    getChannels,
    changeChannel,
    loadChannel,
  };
};

{ useChannelSlice };

} // namespace elizaos
