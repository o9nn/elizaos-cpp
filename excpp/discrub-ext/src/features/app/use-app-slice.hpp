#include "..app/hooks.hpp"
#include "..app/store.hpp"
#include "..classes/message.hpp"
#include "app-slice.hpp"
#include "app-types.hpp"
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

const useAppSlice = () => {
  const dispatch = useAppDispatch();

  const useDiscrubCancelled = (): boolean =>
    useAppSelector((state: RootState) => state.app.discrubCancelled);

  const useDiscrubPaused = (): boolean =>
    useAppSelector((state: RootState) => state.app.discrubPaused);

  const useTask = (): AppTask =>
    useAppSelector((state: RootState) => state.app.task);

  const useSettings = (): AppSettings =>
    useAppSelector((state: RootState) => state.app.settings);

  const state = {
    discrubCancelled: useDiscrubCancelled,
    discrubPaused: useDiscrubPaused,
    task: useTask,
    settings: useSettings,
  };

  const setDiscrubPaused = (value: boolean): void => {
    dispatch(setDiscrubPausedAction(value));
  };

  const setDiscrubCancelled = (value: boolean): void => {
    dispatch(setDiscrubCancelledAction(value));
  };

  const setIsModifying = (value: boolean): void => {
    dispatch(setIsModifyingAction(value));
  };

  const setModifyEntity = (value: Message): void => {
    dispatch(setModifyEntityAction(value));
  };

  const setStatus = (value: string): void => {
    dispatch(setStatusAction(value));
  };

  const resetStatus = (): void => {
    dispatch(resetStatusAction());
  };
  const resetModify = (): void => {
    dispatch(resetModifyAction());
  };

  const setSettings = (settings: AppSettings): void => {
    dispatch(setSettingsAction(settings));
  };

  const checkDiscrubPaused = async (): Promise<void> => {
    await dispatch(checkDiscrubPausedAction());
  };

  /**
   *
   * @param {String} message Message content
   * @param {Number} timeout Number of seconds before message times out
   * @returns
   */
  const setTimeoutMessage = async (
    message: string,
    timeout: number
  ): Promise<void> => {
    await dispatch(setTimeoutMessageAction({ message, timeout }));
  };

  return {
    state,
    setDiscrubPaused,
    setDiscrubCancelled,
    setIsModifying,
    setModifyEntity,
    setStatus,
    resetStatus,
    resetModify,
    checkDiscrubPaused,
    setTimeoutMessage,
    setSettings,
  };
};

{ useAppSlice };

} // namespace elizaos
