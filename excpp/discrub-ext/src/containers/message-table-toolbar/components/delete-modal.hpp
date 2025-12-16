#include "...app/guards.hpp"
#include "...components/cancel-button.hpp"
#include "...components/modal-debug-message.hpp"
#include "...components/pause-button.hpp"
#include "...features/app/app-types.hpp"
#include "...features/message/message-types.hpp"
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

using DeleteModalProps = {
  open: boolean;
  handleClose: () => void;
  handleDeleteMessage: (deleteConfig: DeleteConfiguration) => void;
  selectedRows: string[];
  task: AppTask;
};

const DeleteModal = ({
  open,
  handleClose,
  selectedRows,
  task,
  handleDeleteMessage,
}: DeleteModalProps) => {
  const theme = useTheme();
  const { active, entity, statusText } = task;

  const [deleteConfig, setDeleteConfig] = useState<DeleteConfiguration>({
    attachments: true,
    messages: true,
  });

  useEffect(() => {
    setDeleteConfig({ attachments: true, messages: true });
  }, [open]);

  useEffect(() => {
    if (open && selectedRows.length === 0) {
      handleClose();
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [open, selectedRows]);

  return (
    <Dialog hideBackdrop fullWidth open={open}>
      <DialogTitle>
        <Typography variant="h5">Delete Data</Typography>
        <Typography variant="caption">
          Proceed with caution, this is permanent!
        </Typography>
      </DialogTitle>
      <DialogContent>
        <FormGroup>
          <FormControlLabel
            control={
              <Checkbox
                color="secondary"
                disabled={active}
                defaultChecked
                onChange={(e) => {
                  setDeleteConfig({
                    ...deleteConfig,
                    attachments: e.target.checked,
                  });
                }}
              />
            }
            label="Attachments"
          />
          <FormControlLabel
            control={
              <Checkbox
                color="secondary"
                disabled={active}
                defaultChecked
                onChange={(e) => {
                  setDeleteConfig({
                    ...deleteConfig,
                    messages: e.target.checked,
                  });
                }}
              />
            }
            label="Messages"
          />
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
              <ModalDebugMessage debugMessage={statusText} />
              <Typography sx={{ display: "block" }} variant="caption">
                {`Message ${entity._index} of ${entity._total}`}
              </Typography>
            </>
          )}
        </FormGroup>
      </DialogContent>
      <DialogActions>
        <CancelButton onCancel={handleClose} />
        <PauseButton disabled={!active} />
        <Button
          variant="contained"
          disabled={active}
          onClick={() => handleDeleteMessage(deleteConfig)}
          autoFocus
        >
          Delete
        </Button>
      </DialogActions>
    </Dialog>
  );
};
default DeleteModal;

} // namespace elizaos
