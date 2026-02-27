#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_APP_HOOKS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_APP_HOOKS_H
#include "core.h"
#include "react-redux.h"
#include "react-redux.h"
#include "./store.h"

extern std::function<std::shared_ptr<AppDispatch>()> useAppDispatch;
extern std::shared_ptr<TypedUseSelectorHook<std::shared_ptr<RootState>>> useAppSelector;
#endif
