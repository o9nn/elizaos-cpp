#include "..app/store.hpp"
#include "..classes/guild.hpp"
#include "..classes/role.hpp"
#include "..services/discord-service.hpp"
#include "..utils.hpp"
#include ".channel/channel-slice.hpp"
#include ".dm/dm-types.hpp"
#include ".message/message-slice.hpp"
#include "guild-types.hpp"
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
;
;

const initialState: GuildState = {
  guilds: [],
  selectedGuild: null,
  preFilterUserId: null,
  preFilterUsers: [],
  isLoading: null,
};

const guildSlice = createSlice({
  name: "guild",
  initialState: initialState,
  reducers: {
    setIsLoading: (state, { payload }: { payload: boolean | Maybe }): void => {
      state.isLoading = payload;
    },
    setGuilds: (state, { payload }: { payload: Guild[] }): void => {
      state.guilds = payload;
    },
    setGuild: (state, { payload }: { payload: Snowflake | Maybe }): void => {
      const selectedGuild = state.guilds.find((guild) => guild.id === payload);
      state.selectedGuild = selectedGuild;
    },
    resetGuild: (state): void => {
      state.selectedGuild = null;
      state.preFilterUserId = null;
    },
    setPreFilterUserId: (
      state,
      { payload }: { payload: Snowflake | Maybe }
    ): void => {
      state.preFilterUserId = payload;
    },
    setPreFilterUsers: (
      state,
      { payload }: { payload: PreFilterUser[] }
    ): void => {
      state.preFilterUsers = payload;
    },
  },
});

const {
  setIsLoading,
  setGuilds,
  setGuild,
  resetGuild,
  setPreFilterUserId,
  setPreFilterUsers,
} = guildSlice.actions;

const getRoles =
  (guildId: Snowflake): AppThunk =>
  async (dispatch, getState) => {
    const { settings } = getState().app;
    const { guilds } = getState().guild;
    const { token } = getState().user;
    const guild = guilds.find((g) => g.id === guildId);
    if (guild && !guild.roles && token) {
      const { data, success } = await new DiscordService(settings).fetchRoles(
        guildId,
        token
      );
      if (success && data) {
        const updatedGuilds = guilds.map((g) => {
          if (g.id === guildId) {
            return Object.assign(g, {
              roles: data.map((role) => new Role(role)),
            });
          } else {
            return g;
          }
        });
        dispatch(setGuilds(updatedGuilds));
      }
    }
  };

const getGuilds = (): AppThunk => async (dispatch, getState) => {
  const { settings } = getState().app;
  const { token } = getState().user;
  if (token) {
    dispatch(setIsLoading(true));
    const { success, data } = await new DiscordService(settings).fetchGuilds(
      token
    );
    if (success && data) {
      dispatch(setGuilds(data.map((guild) => new Guild(guild))));
    } else {
      dispatch(setGuilds([]));
    }
    dispatch(setIsLoading(false));
  }
};

const changeGuild =
  (guildId: Snowflake | Maybe): AppThunk =>
  async (dispatch) => {
    if (guildId) {
      dispatch(setGuild(guildId));
      await dispatch(getRoles(guildId));
      await dispatch(getChannels(guildId));
      dispatch(getPreFilterUsers(guildId));
    } else {
      dispatch(resetGuild());
    }
    dispatch(resetChannel());
    dispatch(resetFilters());
    dispatch(resetAdvancedFilters());
    dispatch(resetMessageData());
  };

const getPreFilterUsers =
  (guildId: Snowflake): AppThunk =>
  (dispatch, getState) => {
    const { currentUser } = getState().user;
    const { userMap } = getState().export.exportMaps;
    if (currentUser) {
      const preFilterUsers: PreFilterUser[] = Object.keys(userMap).map(
        (key) => {
          const mapping = userMap[key];
          return { name: mapping.userName, id: key };
        }
      );

      const filteredPreFilters: PreFilterUser[] = [
        ...preFilterUsers.filter(
          (mapping) =>
            mapping.id !== currentUser.id &&
            Boolean(userMap[mapping.id].guilds[guildId]) &&
            mapping.name !== "User Not Found"
        ),
        { id: currentUser.id, name: currentUser.username },
      ].sort((a, b) => sortByProperty(a, b, "name", "asc"));
      dispatch(setPreFilterUsers(filteredPreFilters));
    }
  };

default guildSlice.reducer;

} // namespace elizaos
