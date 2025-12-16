#include ".app/guards.hpp"
#include ".features/app/app-types.hpp"
#include ".utils.hpp"
#include "webhook-embed-mock.hpp"
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

using EmbedModalProps = {
  task: AppTask;
  open: boolean;
  handleClose: () => void;
};

const EmbedModal = ({ task, open, handleClose }: EmbedModalProps) => {
  const { entity } = task || {};

  return (
    <Dialog hideBackdrop fullWidth open={open} onClose={handleClose}>
      <DialogTitle>
        <Typography variant="h5">View Embeds</Typography>
        <Typography variant="caption">
          Embeds associated with this message can be viewed here
        </Typography>
      </DialogTitle>
      <DialogContent sx={{ height: "300px", overflow: "hidden !important" }}>
        <Stack
          sx={{ height: "100%", overflow: "auto", padding: "10px" }}
          pr="25px"
          spacing={2}
        >
          {isMessage(entity) &&
            getRichEmbeds(entity).map((embed) => (
              <WebhookEmbedMock
                alwaysExpanded={false}
                embed={embed}
                message={entity}
              />
            ))}
        </Stack>
      </DialogContent>
      <DialogActions sx={{ minHeight: "57px" }}>
        <Stack
          direction="row"
          justifyContent="flex-end"
          alignItems="center"
          spacing={2}
        >
          <Button variant="contained" onClick={handleClose} color="secondary">
            Close
          </Button>
        </Stack>
      </DialogActions>
    </Dialog>
  );
};
default EmbedModal;

} // namespace elizaos
