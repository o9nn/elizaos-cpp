#include ".features/app/use-app-slice.hpp"
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

using CancelButtonProps = {
  disabled?: boolean;
  onCancel?: () => void;
};

const CancelButton = ({ onCancel, disabled = false }: CancelButtonProps) => {
  const {
    state: appState,
    setDiscrubCancelled,
    setDiscrubPaused,
  } = useAppSlice();
  const discrubCancelled = appState.discrubCancelled();

  const handleCancel = () => {
    if (onCancel) {
      onCancel();
    } else {
      setDiscrubCancelled(true);
      setDiscrubPaused(false);
    }
  };

  return (
    <Button
      disabled={discrubCancelled || disabled}
      color="secondary"
      variant="contained"
      onClick={handleCancel}
    >
      Cancel
    </Button>
  );
};

default CancelButton;

} // namespace elizaos
