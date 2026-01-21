#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_PURGE_USE-PURGE-SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_PURGE_USE-PURGE-SLICE_H
#include "core.h"
#include "../../app/store.h"
#include "./purge-slice.h"
using setIsLoadingAction = setIsLoading;
using purgeAction = purge;
#include "../../app/hooks.h"
#include "../../classes/channel.h"
using Channel = _default;

extern std::function<object()> usePurgeSlice;
#endif
