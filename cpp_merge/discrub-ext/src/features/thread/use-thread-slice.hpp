#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_FEATURES_THREAD_USE_THREAD_SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_FEATURES_THREAD_USE_THREAD_SLICE_H
#include "core.hpp"
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
// Using alias removed (invalid transpilation)
#include "../../classes/message.h"
// Using alias removed (invalid transpilation)

extern std::function<object()> useThreadSlice;
#endif
