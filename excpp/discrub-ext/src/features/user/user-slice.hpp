#include "..app/store.hpp"
#include "..classes/user.hpp"
#include "..services/chrome-service.hpp"
#include "..services/discord-service.hpp"
#include "user-types.hpp"
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

const initialState: UserState = {
  currentUser: null,
  token: null,
  isLoading: null,
};

const userSlice = createSlice({
  name: "user",
  initialState: initialState,
  reducers: {
    setIsLoading: (state, { payload }: { payload: boolean }): void => {
      state.isLoading = payload;
    },
    setToken: (state, { payload }: { payload: string | Maybe }): void => {
      state.token = payload;
    },
    setCurrentUser: (state, { payload }: { payload: User }): void => {
      state.currentUser = payload;
    },
  },
});

const { setIsLoading, setToken, setCurrentUser } = userSlice.actions;

const getUserData = (): AppThunk => async (dispatch, getState) => {
  const { settings } = getState().app;
  dispatch(setIsLoading(true));
  const chromeCallback = async (userToken: string) => {
    if (userToken) {
      const { success, data } = await new DiscordService(
        settings
      ).fetchUserData(userToken);
      if (success && data) {
        dispatch(setCurrentUser(data));
        dispatch(setToken(userToken));
      }
    }
    dispatch(setIsLoading(false));
  };
  return sendChromeMessage("GET_TOKEN", chromeCallback);
};

const getUserDataManaully =
  (userToken: string): AppThunk<Promise<boolean>> =>
  async (dispatch, getState) => {
    const { settings } = getState().app;
    if (userToken) {
      const { data, success } = await new DiscordService(
        settings
      ).fetchUserData(userToken);

      if (success && data) {
        dispatch(setToken(userToken));
        dispatch(setCurrentUser(data));
        return true;
      } else {
        dispatch(setToken(undefined));
        dispatch(setIsLoading(false));
        return false;
      }
    }
    return false;
  };

default userSlice.reducer;

} // namespace elizaos
