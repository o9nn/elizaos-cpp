#include "..app/hooks.hpp"
#include "..app/store.hpp"
#include "..classes/guild.hpp"
#include ".dm/dm-types.hpp"
#include "guild-slice.hpp"
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

const useGuildSlice = () => {
  const dispatch = useAppDispatch();

  const useGuilds = (): Guild[] =>
    useAppSelector((state: RootState) => state.guild.guilds);

  const useSelectedGuild = (): Guild | Maybe =>
    useAppSelector((state: RootState) => {
      const { selectedGuild } = state.guild;
      if (selectedGuild) {
        return selectedGuild;
      }
    });

  const usePreFilterUserId = (): Snowflake | Maybe =>
    useAppSelector((state: RootState) => state.guild.preFilterUserId);

  const usePreFilterUsers = (): PreFilterUser[] =>
    useAppSelector((state: RootState) => state.guild.preFilterUsers);

  const useIsLoading = (): boolean | Maybe =>
    useAppSelector((state: RootState) => state.guild.isLoading);

  const state = {
    guilds: useGuilds,
    selectedGuild: useSelectedGuild,
    preFilterUserId: usePreFilterUserId,
    preFilterUsers: usePreFilterUsers,
    isLoading: useIsLoading,
  };

  const setIsLoading = (value: boolean): void => {
    dispatch(setIsLoadingAction(value));
  };

  const setGuilds = (guilds: Guild[]) => {
    dispatch(setGuildsAction(guilds));
  };

  const setGuild = (guildId: Snowflake | Maybe) => {
    dispatch(setGuildAction(guildId));
  };

  const resetGuild = () => {
    dispatch(resetGuildAction());
  };

  const setPreFilterUserId = (userId: Snowflake | Maybe) => {
    dispatch(setPreFilterUserIdAction(userId));
  };

  const setPreFilterUsers = (preFilterUsers: PreFilterUser[]) => {
    dispatch(setPreFilterUsersAction(preFilterUsers));
  };

  const getRoles = (guildId: Snowflake) => {
    dispatch(getRolesAction(guildId));
  };

  const getGuilds = () => {
    dispatch(getGuildsAction());
  };

  const changeGuild = (guildId: Snowflake | Maybe) => {
    dispatch(changeGuildAction(guildId));
  };

  const getPreFilterUsers = (guildId: Snowflake) => {
    dispatch(getPreFilterUsersAction(guildId));
  };

  return {
    state,
    setIsLoading,
    setGuilds,
    setGuild,
    resetGuild,
    setPreFilterUserId,
    setPreFilterUsers,
    getRoles,
    getGuilds,
    changeGuild,
    getPreFilterUsers,
  };
};

{ useGuildSlice };

} // namespace elizaos
