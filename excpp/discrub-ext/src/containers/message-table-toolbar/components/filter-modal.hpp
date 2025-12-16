#include "...classes/channel.hpp"
#include "...features/message/message-types.hpp"
#include "filter-component.hpp"
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

using FilterModalProps = {
  open: boolean;
  handleModalToggle: () => void;
  isDm: boolean;
  threads: Channel[];
  handleFilterUpdate: (filter: Filter) => void;
  handleResetFilters: () => void;
  filters: Filter[];
};

const FilterModal = ({
  handleModalToggle,
  open,
  isDm,
  threads,
  handleFilterUpdate,
  handleResetFilters,
  filters,
}: FilterModalProps) => {
  const handleReset = () => {
    handleResetFilters();
    handleModalToggle();
  };

  return (
    <Dialog hideBackdrop fullWidth open={open} keepMounted={!!filters.length}>
      <DialogTitle>
        <Typography variant="h5">Quick Filtering</Typography>
      </DialogTitle>
      <DialogContent>
        <FilterComponent
          isDm={isDm}
          handleFilterUpdate={handleFilterUpdate}
          threads={threads}
        />
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
default FilterModal;

} // namespace elizaos
