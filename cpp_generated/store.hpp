#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_APP_STORE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_APP_STORE_H
#include "core.h"
#include "@reduxjs/toolkit.h"
#include "../features/app/app-slice.h"
using appReducer = _default;
#include "../features/channel/channel-slice.h"
using channelReducer = _default;
#include "../features/user/user-slice.h"
using userReducer = _default;
#include "../features/thread/thread-slice.h"
using threadReducer = _default;
#include "../features/dm/dm-slice.h"
using dmReducer = _default;
#include "../features/guild/guild-slice.h"
using guildReducer = _default;
#include "../features/relationship/relationship-slice.h"
using relationshipReducer = _default;
#include "../features/purge/purge-slice.h"
using purgeReducer = _default;
#include "../features/export/export-slice.h"
using exportReducer = _default;
#include "../features/message/message-slice.h"
using messageReducer = _default;

typedef ReturnType<> RootState;

typedef  AppDispatch;

template <typename ReturnType>
using AppThunk = ThunkAction<ReturnType, RootState, any, std::shared_ptr<Action<string>>>;


extern any store;
#endif
