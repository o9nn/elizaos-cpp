#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_DM_USE-DM-SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_DM_USE-DM-SLICE_H
#include "core.h"
#include "../../app/store.h"
#include "./dm-slice.h"
using setIsLoadingAction = setIsLoading;
using setDmsAction = setDms;
using resetDmAction = resetDm;
using setPreFilterUserIdAction = setPreFilterUserId;
using getDmsAction = getDms;
using mutateSelectedDmsAction = mutateSelectedDms;
#include "../../app/hooks.h"
#include "./dm-types.h"
#include "../../classes/channel.h"
using Channel = _default;

extern std::function<object()> useDmSlice;
#endif
