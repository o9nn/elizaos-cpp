#include "..app/hooks.hpp"
#include "..app/store.hpp"
#include "..classes/user.hpp"
#include "user-slice.hpp"
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

const useUserSlice = () => {
  const dispatch = useAppDispatch();

  const useCurrentUser = (): User | Maybe =>
    useAppSelector((state: RootState) => state.user.currentUser);

  const useToken = (): string | Maybe =>
    useAppSelector((state: RootState) => state.user.token);

  const useIsLoading = (): boolean | Maybe =>
    useAppSelector((state: RootState) => state.user.isLoading);

  const state = {
    currentUser: useCurrentUser,
    token: useToken,
    isLoading: useIsLoading,
  };

  const setIsLoading = (value: boolean): void => {
    dispatch(setIsLoadingAction(value));
  };

  const setToken = (value: string | Maybe): void => {
    dispatch(setTokenAction(value));
  };

  const setCurrentUser = (value: User): void => {
    dispatch(setCurrentUserAction(value));
  };

  const getUserData = (): void => {
    dispatch(getUserDataAction());
  };

  const getUserDataManaully = async (token: string): Promise<boolean> => {
    return dispatch(getUserDataManaullyAction(token));
  };

  return {
    state,
    setIsLoading,
    setToken,
    setCurrentUser,
    getUserData,
    getUserDataManaully,
  };
};

{ useUserSlice };

} // namespace elizaos
