#include "..app/hooks.hpp"
#include "..app/store.hpp"
#include "..classes/channel.hpp"
#include "purge-slice.hpp"
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

const usePurgeSlice = () => {
  const dispatch = useAppDispatch();

  const useIsLoading = (): boolean | Maybe =>
    useAppSelector((state: RootState) => state.purge.isLoading);

  const usePurgeChannel = (): Channel | null =>
    useAppSelector((state: RootState) => state.purge.purgeChannel);

  const state = {
    isLoading: useIsLoading,
    purgeChannel: usePurgeChannel,
  };

  const setIsLoading = (value: boolean): void => {
    dispatch(setIsLoadingAction(value));
  };

  /**
   * Iterates through the provided array and deletes every message from each Channel.
   */
  const purge = (channels: Channel[]) => {
    dispatch(purgeAction(channels));
  };

  return {
    state,
    setIsLoading,
    purge,
  };
};

{ usePurgeSlice };

} // namespace elizaos
