#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_FEATURES_APP_USE_APP_SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_FEATURES_APP_USE_APP_SLICE_H
#include "core.hpp"
#include "../../app/store.h"
#include "./app-slice.h"
using setDiscrubPausedAction = setDiscrubPaused;
using setDiscrubCancelledAction = setDiscrubCancelled;
using setIsModifyingAction = setIsModifying;
using setModifyEntityAction = setModifyEntity;
using setStatusAction = setStatus;
using resetStatusAction = resetStatus;
using resetModifyAction = resetModify;
using checkDiscrubPausedAction = checkDiscrubPaused;
using setTimeoutMessageAction = setTimeoutMessage;
using setSettingsAction = setSettings;
#include "./app-types.h"
#include "../../classes/message.h"
// Using alias removed (invalid transpilation)
#include "../../app/hooks.h"

extern std::function<object()> useAppSlice;
#endif
