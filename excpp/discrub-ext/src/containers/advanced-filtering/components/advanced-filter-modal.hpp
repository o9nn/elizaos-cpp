#include "...components/before-and-after-fields.hpp"
#include "...components/prefilter-user.hpp"
#include "has-type.hpp"
#include "message-contains.hpp"
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

using AdvancedFilterModalProps = {
  open: boolean;
  handleModalToggle: () => void;
  isDm: boolean;
  handleResetFilters: () => void;
  filtersActive: boolean;
};

const AdvancedFilterModal = ({
  handleModalToggle,
  open,
  isDm,
  handleResetFilters,
  filtersActive,
}: AdvancedFilterModalProps) => {
  const handleReset = () => {
    handleResetFilters();
    handleModalToggle();
  };

  return (
    <Dialog hideBackdrop fullWidth open={open} keepMounted={filtersActive}>
      <DialogTitle>
        <Typography variant="h5">Advanced Filtering</Typography>
      </DialogTitle>
      <DialogContent>
        <Stack direction="column" spacing={1}>
          <PrefilterUser isDm={isDm} />
          <BeforeAndAfterFields disabled={false} />
          <MessageContains disabled={false} />
          <HasType disabled={false} />
        </Stack>
      </DialogContent>
      <DialogActions>
        <Button color="primary" variant="contained" onClick={handleReset}>
          Reset
        </Button>
        <Button
          color="secondary"
          variant="contained"
          onClick={handleModalToggle}
        >
          Close
        </Button>
      </DialogActions>
    </Dialog>
  );
};
default AdvancedFilterModal;

} // namespace elizaos
