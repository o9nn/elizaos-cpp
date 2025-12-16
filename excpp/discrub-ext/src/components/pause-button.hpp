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
;
;

using PauseButtonProps = {
  disabled?: boolean;
};

const PauseButton = ({ disabled = false }: PauseButtonProps) => {
  const { state: appState, setDiscrubPaused } = useAppSlice();
  const discrubCancelled = appState.discrubCancelled();
  const discrubPaused = appState.discrubPaused();

  return (
    <>
      <Button
        startIcon={discrubPaused ? <PlayArrowIcon /> : <PauseIcon />}
        disabled={discrubCancelled || disabled}
        color="secondary"
        variant="contained"
        onClick={() => setDiscrubPaused(!discrubPaused)}
      >
        {discrubPaused ? "Resume" : "Pause"}
      </Button>

      <Snackbar
        anchorOrigin={{ vertical: "top", horizontal: "center" }}
        open={discrubPaused}
      >
        <Alert variant="filled" severity="warning">
          Paused
        </Alert>
      </Snackbar>
    </>
  );
};

default PauseButton;

} // namespace elizaos
