#include "...app/guards.hpp"
#include "...classes/message.hpp"
#include "...components/cancel-button.hpp"
#include "...components/modal-debug-message.hpp"
#include "...components/pause-button.hpp"
#include "...features/app/app-types.hpp"
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

using EditModalProps = {
  open: boolean;
  task: AppTask;
  handleEditMessage: (editText: string) => void;
  handleClose: () => void;
};

const EditModal = ({
  task,
  handleEditMessage,
  handleClose,
  open,
}: EditModalProps) => {
  const { active, entity, statusText } = task;

  const [updateText, setUpdateText] = useState("");

  useEffect(() => {
    if (open) {
      setUpdateText("");
    }
  }, [open]);

  return (
    <Dialog hideBackdrop fullWidth open={open}>
      <DialogTitle>
        <Typography variant="h5">Edit Data</Typography>
        <Typography variant="caption">
          Proceed with caution, this is permanent!
        </Typography>
      </DialogTitle>
      <DialogContent>
        <TextField
          fullWidth
          variant="filled"
          disabled={active}
          label="Update Text"
          value={updateText}
          onChange={(e) => setUpdateText(e.target.value)}
        />
        {active && isMessage(entity) && (
          <>
            <Stack
              my={1}
              flexDirection="column"
              justifyContent="center"
              alignItems="center"
              gap="5px"
            >
              <Box
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
              <LinearProgress sx={{ width: "100%" }} />
              <SouthIcon />
              <Box
                sx={{
                  minHeight: "50px",
                  maxHeight: "50px",
                  overflowX: "hidden",
                  overflowY: "auto",
                  width: "100%",
                }}
              >
                <MessageMock
                  browserView
                  index={entity.id}
                  message={new Message({ ...entity, content: updateText })}
                />
              </Box>
            </Stack>
            <ModalDebugMessage debugMessage={statusText} />
          </>
        )}
      </DialogContent>
      <DialogActions>
        <CancelButton onCancel={handleClose} />
        <PauseButton disabled={!active} />
        <Button
          variant="contained"
          disabled={updateText.length === 0 || active}
          onClick={() => handleEditMessage(updateText)}
          autoFocus
        >
          Edit
        </Button>
      </DialogActions>
    </Dialog>
  );
};
default EditModal;

} // namespace elizaos
