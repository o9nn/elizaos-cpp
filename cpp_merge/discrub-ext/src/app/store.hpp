#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_APP_STORE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_APP_STORE_H
#include "core.hpp"
// External dependency removed
#include "../features/app/app-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/channel/channel-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/user/user-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/thread/thread-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/dm/dm-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/guild/guild-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/relationship/relationship-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/purge/purge-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/export/export-slice.h"
// Using alias removed (invalid transpilation)
#include "../features/message/message-slice.h"
// Using alias removed (invalid transpilation)

typedef ReturnType<> RootState;

typedef  AppDispatch;

template <typename ReturnType>
using AppThunk = ThunkAction<ReturnType, RootState, any, std::shared_ptr<Action<string>>>;


extern any store;
#endif
