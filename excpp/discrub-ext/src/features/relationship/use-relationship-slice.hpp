#include "..app/hooks.hpp"
#include "..app/store.hpp"
#include "relationship-slice.hpp"
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

const useRelationshipSlice = () => {
  const dispatch = useAppDispatch();

  const useIsLoading = (): boolean | Maybe =>
    useAppSelector((state: RootState) => state.relationship.isLoading);

  const useFriends = (): unknown[] =>
    useAppSelector((state: RootState) => state.relationship.friends);

  const state = {
    isLoading: useIsLoading,
    friends: useFriends,
  };

  const setIsLoading = (value: boolean): void => {
    dispatch(setIsLoadingAction(value));
  };

  const setFriends = (friends: unknown[]) => {
    dispatch(setFriendsAction(friends));
  };

  const getFriends = () => {
    dispatch(getFriendsAction());
  };

  const addFriend = (username: string, discriminator: string) => {
    dispatch(addFriendAction({ username, discriminator }));
  };

  const deleteFriend = (userId: Snowflake) => {
    dispatch(deleteFriendAction(userId));
  };

  const openDm = (userId: Snowflake) => {
    dispatch(openDmAction(userId));
  };

  return {
    state,
    setIsLoading,
    setFriends,
    getFriends,
    addFriend,
    deleteFriend,
    openDm,
  };
};

{ useRelationshipSlice };

} // namespace elizaos
