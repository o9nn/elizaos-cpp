#include "components/purge-modal.hpp"
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

using PurgeButtonProps = {
  isDm?: boolean;
  disabled?: boolean;
};

const PurgeButton = ({ isDm = false, disabled = false }: PurgeButtonProps) => {
  const [dialogOpen, setDialogOpen] = useState(false);
  return (
    <>
      <Button
        disabled={disabled}
        onClick={() => {
          setDialogOpen(true);
        }}
        variant="contained"
      >
        Purge
      </Button>
      <PurgeModal
        dialogOpen={dialogOpen}
        setDialogOpen={setDialogOpen}
        isDm={isDm}
      />
    </>
  );
};

default PurgeButton;

} // namespace elizaos
