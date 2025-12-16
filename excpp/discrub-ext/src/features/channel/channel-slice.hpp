#include "..app/store.hpp"
#include "..classes/channel.hpp"
#include "..enum/channel-type.hpp"
#include "..services/discord-service.hpp"
#include ".guild/guild-slice.hpp"
#include ".message/message-slice.hpp"
#include "channel-types.hpp"
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
;
;
;
;

const initialState: ChannelState = {
  channels: [],
  selectedChannel: null,
  isLoading: null,
  selectedExportChannels: [], // Array of channel ID's, used for exporting Guild}
};

const channelSlice = createSlice({
  name: "channel",
  initialState: initialState,
  reducers: {
    setIsLoading: (state, { payload }: { payload: boolean }): void => {
      state.isLoading = payload;
    },
    setChannels: (state, { payload }: { payload: Channel[] }): void => {
      state.channels = payload;
    },
    setChannel: (state, { payload }: { payload: Snowflake | null }): void => {
      const selectedChannel = state.channels.find(
        (channel) => channel.id === payload
      );
      state.selectedChannel = selectedChannel || null;
    },
    resetChannel: (state): void => {
      state.selectedChannel = null;
    },
    setSelectedExportChannels: (
      state,
      { payload }: { payload: Snowflake[] }
    ): void => {
      state.selectedExportChannels = payload;
    },
  },
});

const {
  setIsLoading,
  setChannels,
  setChannel,
  resetChannel,
  setSelectedExportChannels,
} = channelSlice.actions;

const getChannels =
  (guildId: Snowflake): AppThunk =>
  async (dispatch, getState) => {
    const { settings } = getState().app;
    const { token } = getState().user;
    if (guildId && token) {
      dispatch(setIsLoading(true));
      const { success, data } = await new DiscordService(
        settings
      ).fetchChannels(token, guildId);
      if (success && data) {
        dispatch(
          setChannels(data.filter((c) => c.type !== ChannelType.GUILD_CATEGORY))
        );
      } else {
        dispatch(setChannels([]));
      }
      dispatch(setIsLoading(false));
    }
  };

const changeChannel =
  (channelId: Snowflake | null): AppThunk =>
  async (dispatch) => {
    if (!channelId) {
      dispatch(resetAdvancedFilters());
      dispatch(setPreFilterUserId(null));
    }
    dispatch(resetFilters());
    dispatch(resetMessageData());
    dispatch(setChannel(channelId));
  };

const loadChannel =
  (channelId: Snowflake): AppThunk =>
  async (dispatch, getState) => {
    const { settings } = getState().app;
    const { selectedGuild } = getState().guild;
    const { token } = getState().user;
    const { channels } = getState().channel;

    if (token && channelId && !channels.map((c) => c.id).includes(channelId)) {
      const { data, success } = await new DiscordService(settings).fetchChannel(
        token,
        channelId
      );
      if (success && data && data.guild_id === selectedGuild?.id) {
        dispatch(setChannels([...channels, data]));
      }
    }
  };

default channelSlice.reducer;

} // namespace elizaos
