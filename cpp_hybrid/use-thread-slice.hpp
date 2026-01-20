#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_THREAD_USE-THREAD-SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_THREAD_USE-THREAD-SLICE_H
#include "core.h"
#include "../../app/store.h"
#include "./thread-slice.h"
using setThreadsAction = setThreads;
using resetThreadsAction = resetThreads;
using getArchivedThreadsAction = getArchivedThreads;
using unarchiveThreadAction = unarchiveThread;
using liftThreadRestrictionsAction = liftThreadRestrictions;
using getThreadsFromMessagesAction = getThreadsFromMessages;
#include "../../app/hooks.h"
#include "../../classes/channel.h"
using Channel = _default;
#include "../../classes/message.h"
using Message = _default;

extern std::function<object()> useThreadSlice;
#endif
