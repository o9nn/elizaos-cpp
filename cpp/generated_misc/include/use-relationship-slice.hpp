#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_RELATIONSHIP_USE-RELATIONSHIP-SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_RELATIONSHIP_USE-RELATIONSHIP-SLICE_H
#include "core.h"
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
