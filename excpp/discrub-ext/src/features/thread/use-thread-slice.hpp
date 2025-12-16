#include "..app/hooks.hpp"
#include "..app/store.hpp"
#include "..classes/channel.hpp"
#include "..classes/message.hpp"
#include "thread-slice.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;
;

const useThreadSlice = () => {
  const dispatch = useAppDispatch();

  const useThreads = (): Channel[] =>
    useAppSelector((state: RootState) => state.thread.threads);

  const state = {
    threads: useThreads,
  };

  const setThreads = (value: Channel[]): void => {
    dispatch(setThreadsAction(value));
  };

  const resetThreads = (): void => {
    dispatch(resetThreadsAction());
  };

  const getArchivedThreads = async (
    channelId: string,
    knownThreads: Channel[]
  ): Promise<Channel[]> => {
    const threads = await dispatch(
      getArchivedThreadsAction({ channelId, knownThreads })
    );
    return threads;
  };

  const getThreadsFromMessages = (
    messages: Message[],
    knownThreads: Channel[]
  ): Channel[] => {
    return getThreadsFromMessagesAction({ messages, knownThreads });
  };

  const unarchiveThread = async (
    threadId: string
  ): Promise<Maybe | Channel> => {
    const thread = await dispatch(unarchiveThreadAction(threadId));
    return thread;
  };

  /**
   *
   * @param {string} channelId The Id of the Thread to lift locked/archived restrictions from
   * @param {Array} noPermissionThreadIds Optional array of Thread Ids that could not be unlocked
   * @returns An updated array of Thread Ids that the User does not have permission to modify
   */
  const liftThreadRestrictions = async (
    channelId: string,
    noPermissionThreadIds: string[]
  ): Promise<string[]> => {
    const noPermissionIds = await dispatch(
      liftThreadRestrictionsAction({ channelId, noPermissionThreadIds })
    );

    return noPermissionIds;
  };

  return {
    state,
    setThreads,
    resetThreads,
    getArchivedThreads,
    getThreadsFromMessages,
    unarchiveThread,
    liftThreadRestrictions,
  };
};

{ useThreadSlice };

} // namespace elizaos
