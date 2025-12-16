#include "...app/guards.hpp"
#include "...components/cancel-button.hpp"
#include "...components/modal-debug-message.hpp"
#include "...components/pause-button.hpp"
#include "...components/prefilter-user.hpp"
#include "...features/app/use-app-slice.hpp"
#include "...features/channel/use-channel-slice.hpp"
#include "...features/dm/use-dm-slice.hpp"
#include "...features/guild/use-guild-slice.hpp"
#include "...features/message/use-message-slice.hpp"
#include "...features/purge/use-purge-slice.hpp"
#include "...features/user/use-user-slice.hpp"
#include "..message-mock/message-mock.hpp"
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
;
;
;
;
;
;
;
;
;
;

using PurgeModalProps = {
  dialogOpen: boolean;
  setDialogOpen: (val: boolean) => void;
  isDm?: boolean;
};

const PurgeModal = ({
  dialogOpen,
  setDialogOpen,
  isDm = false,
}: PurgeModalProps) => {
  const theme = useTheme();

  const { state: messageState } = useMessageSlice();
  const messagesLoading = messageState.isLoading();

  const { state: channelState, resetChannel } = useChannelSlice();
  const channels = channelState.channels();

  const { state: guildState, setPreFilterUserId } = useGuildSlice();
  const preFilterUserId = guildState.preFilterUserId();

  const { state: dmState, setPreFilterUserId: setDmPreFilterUserId } =
    useDmSlice();
  const selectedDms = dmState.selectedDms();

  const { state: userState } = useUserSlice();
  const currentUser = userState.currentUser();

  const {
    state: appState,
    setDiscrubCancelled,
    setDiscrubPaused,
    resetModify,
    setIsModifying,
  } = useAppSlice();
  const task = appState.task();

  const { purge } = usePurgeSlice();

  const { active, entity, statusText } = task;

  const finishedPurge = !active && entity;

  const deleteType = isDm
    ? `DM${selectedDms.length > 1 ? "'s" : ""}`
    : "Server";

  useEffect(() => {
    if (dialogOpen) {
      resetModify();
      setIsModifying(false);
      if (!isDm) {
        setPreFilterUserId(currentUser?.id);
      }
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [dialogOpen]);

  const handleClose = async () => {
    if (!!entity || active) {
      // We are actively deleting, we need to send a cancel request
      setDiscrubCancelled(true);
    }
    if (isDm) {
      setDmPreFilterUserId(null);
    } else {
      setPreFilterUserId(null);
      resetChannel();
    }
    setDiscrubPaused(false);
    setDialogOpen(false);
  };

  const dialogBtnDisabled = Boolean(
    active || entity || (!isDm && !preFilterUserId)
  );

  const dmDialogText = `Are you sure you want to purge ${
    selectedDms.length > 1 ? "these DM's" : "this DM"
  }? All of your messages will be deleted.`;

  const guildDialogText =
    "Are you sure you want to purge this Server? All messages in every Channel will be deleted for yourself or a given User Id.";

  const handlePurge = () => {
    if (isDm && !!selectedDms.length) {
      purge([...selectedDms]);
    } else if (!isDm && channels.length) {
      purge([...channels]);
    }
  };

  return (
    <Dialog hideBackdrop open={dialogOpen}>
      <DialogTitle>
        {active && entity && `Purging ${deleteType}`}
        {finishedPurge && `${deleteType} Purged`}
        {!finishedPurge && !active && `Purge ${deleteType}`}
      </DialogTitle>
      <DialogContent>
        <Stack
          direction="column"
          justifyContent="center"
          alignItems="center"
          spacing={2}
        >
          <Stack
            direction="row"
            justifyContent="center"
            alignItems="center"
            spacing={2}
          >
            {!finishedPurge && (
              <WarningAmberIcon
                sx={{ color: theme.palette.warning.main }}
                fontSize="large"
              />
            )}
            {finishedPurge && <ThumbUpIcon fontSize="large" />}
            <DialogContentText>
              <Typography variant="body2">
                {finishedPurge && `${deleteType} was successfully purged!`}
                {!finishedPurge && (isDm ? dmDialogText : guildDialogText)}
              </Typography>
            </DialogContentText>
          </Stack>
          {!isDm && !finishedPurge && (
            <PrefilterUser
              isDm={false}
              purge
              disabled={Boolean(messagesLoading)}
            />
          )}
          {active && isMessage(entity) && (
            <>
              <Box
                my={1}
                sx={{
                  minHeight: "50px",
                  maxHeight: "50px",
                  overflowX: "hidden",
                  overflowY: "auto",
                  width: "100%",
                }}
              >
                <MessageMock browserView index={entity.id} message={entity} />
              </Box>

              <Stack
                flexDirection="column"
                justifyContent="center"
                alignItems="center"
                gap="5px"
                width="100%"
              >
                <LinearProgress sx={{ width: "100%" }} />
                <SouthIcon />
                <DeleteSweepIcon sx={{ color: theme.palette.error.main }} />
              </Stack>
              <Typography sx={{ display: "block" }} variant="caption">
                {entity?._index
                  ? `Message ${entity._index} of ${entity._total}`
                  : ""}
              </Typography>
            </>
          )}
          <ModalDebugMessage debugMessage={statusText} />
        </Stack>
      </DialogContent>
      <DialogActions>
        <CancelButton onCancel={handleClose} />
        <PauseButton disabled={!active} />
        {!finishedPurge && (
          <Button
            disabled={dialogBtnDisabled}
            variant="contained"
            onClick={handlePurge}
          >
            Purge
          </Button>
        )}
      </DialogActions>
    </Dialog>
  );
};

default PurgeModal;

} // namespace elizaos
