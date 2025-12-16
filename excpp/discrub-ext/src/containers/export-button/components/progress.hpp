#include "...features/app/use-app-slice.hpp"
#include "...features/export/use-export-slice.hpp"
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

const Progress = () => {
  const { state: appState } = useAppSlice();
  const task = appState.task();
  const { statusText } = task || {};

  const { state: exportState } = useExportSlice();
  const name = exportState.name();

  return (
    <Stack
      direction="column"
      justifyContent="center"
      alignItems="center"
      spacing={2}
      sx={{ minWidth: "300px" }}
    >
      <Typography>{name}</Typography>
      <LinearProgress sx={{ width: "100%", m: 1 }} />
      <Typography variant="caption">{statusText}</Typography>
    </Stack>
  );
};

default Progress;

} // namespace elizaos
