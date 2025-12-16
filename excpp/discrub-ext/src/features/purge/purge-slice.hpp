#include "..app/store.hpp"
#include "..classes/channel.hpp"
#include "..classes/message.hpp"
#include "..utils.hpp"
#include ".app/app-slice.hpp"
#include ".message/message-slice.hpp"
#include ".thread/thread-slice.hpp"
#include "purge-types.hpp"
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
;
;
;
;

const initialState: PurgeState = {
  isLoading: null,
  purgeChannel: null,
};

const purgeSlice = createSlice({
  name: "purge",
  initialState: initialState,
  reducers: {
    setIsLoading: (state, { payload }: { payload: boolean }): void => {
      state.isLoading = payload;
    },
    setPurgeChannel: (
      state,
      { payload }: { payload: Channel | null }
    ): void => {
      state.purgeChannel = payload;
    },
  },
});

const { setIsLoading, setPurgeChannel } = purgeSlice.actions;

const purge =
  (channels: Channel[]): AppThunk =>
  async (dispatch, getState) => {
    const { currentUser } = getState().user;
    if (currentUser) {
      const { selectedGuild, preFilterUserId } = getState().guild;
      dispatch(setIsModifying(true));
      for (const entity of channels) {
        const { discrubCancelled } = getState().app;
        if (discrubCancelled) break;
        await dispatch(checkDiscrubPaused());
        dispatch(setPurgeChannel(entity));
        dispatch(setModifyEntity(null));
        await dispatch(
          notify({ message: "Searching for messages...", timeout: 1 })
        );
        dispatch(resetMessageData());
        await dispatch(
          getMessageData(selectedGuild?.id, entity.id, {
            preFilterUserId: isDm(entity) ? currentUser.id : preFilterUserId,
            excludeReactions: true,
          })
        );
        const selectedMessages: Message[] = getState().message.messages;
        const selectedCount = selectedMessages.length;
        if (selectedCount === 0) {
          await dispatch(notify({ message: "Still searching...", timeout: 1 }));
        }

        let threadIds: Snowflake[] = []; // Thread Id's that we do NOT have permission to modify
        for (const [count, currentRow] of selectedMessages.entries()) {
          const { discrubCancelled } = getState().app;
          if (discrubCancelled) break;
          await dispatch(checkDiscrubPaused());

          threadIds = await dispatch(
            liftThreadRestrictions({
              channelId: currentRow.channel_id,
              noPermissionThreadIds: threadIds,
            })
          );

          const modifyEntity = Object.assign(new Message({ ...currentRow }), {
            _index: count + 1,
            _total: selectedCount,
          });

          dispatch(setModifyEntity(modifyEntity));

          const isMissingPermission = threadIds.some(
            (tId) => tId === currentRow.channel_id
          );
          if (isMissingPermission) {
            await dispatch(
              notify({
                message: "Permission missing for message, skipping delete",
                timeout: 1,
              })
            );
          } else if (isRemovableMessage(currentRow)) {
            const success = await dispatch(deleteMessage(currentRow));
            if (!success) {
              await dispatch(
                notify({
                  message: "You do not have permission to modify this message!",
                  timeout: 0.5,
                })
              );
            }
          }
        }
      }
      dispatch(resetModify());
      dispatch(resetMessageData());
      dispatch(setDiscrubCancelled(false));
      dispatch(setPurgeChannel(null));
    }
  };

default purgeSlice.reducer;

} // namespace elizaos
