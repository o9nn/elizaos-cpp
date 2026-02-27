#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_FEATURES_RELATIONSHIP_USE_RELATIONSHIP_SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_FEATURES_RELATIONSHIP_USE_RELATIONSHIP_SLICE_H
#include "core.hpp"
#include "../../app/store.h"
#include "./relationship-slice.h"
using setIsLoadingAction = setIsLoading;
using setFriendsAction = setFriends;
using getFriendsAction = getFriends;
using addFriendAction = addFriend;
using deleteFriendAction = deleteFriend;
using openDmAction = openDm;
#include "../../app/hooks.h"

extern std::function<object()> useRelationshipSlice;
#endif
