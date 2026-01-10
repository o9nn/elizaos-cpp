#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_USER_USE-USER-SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_USER_USE-USER-SLICE_H
#include "core.h"
#include "../../app/store.h"
#include "./user-slice.h"
using setIsLoadingAction = setIsLoading;
using setTokenAction = setToken;
using setCurrentUserAction = setCurrentUser;
using getUserDataAction = getUserData;
using getUserDataManaullyAction = getUserDataManaully;
#include "../../app/hooks.h"
#include "../../classes/user.h"

extern std::function<object()> useUserSlice;
#endif
