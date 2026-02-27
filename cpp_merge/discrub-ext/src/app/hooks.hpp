#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_APP_HOOKS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_APP_HOOKS_H
#include "core.hpp"
#include "react-redux.h"
#include "react-redux.h"
#include "./store.h"

extern std::function<std::shared_ptr<AppDispatch>()> useAppDispatch;
extern std::shared_ptr<TypedUseSelectorHook<std::shared_ptr<RootState>>> useAppSelector;
#endif
