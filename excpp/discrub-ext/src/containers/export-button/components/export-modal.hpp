#include "...enum/export-type.hpp"
#include "export-modal-actions.hpp"
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

using ExportModalProps = {
  handleExportSelected: (val: ExportType) => void;
  onCancel: () => void;
  dialogOpen: boolean;
  exportDisabled: boolean;
  pauseDisabled: boolean;
  ContentComponent: React.ReactNode;
  dialogTitle: string;
  getTooltipDescription: (exportType: ExportType) => string;
};

const ExportModal = ({
  dialogOpen,
  handleExportSelected,
  exportDisabled,
  pauseDisabled,
  onCancel,
  ContentComponent,
  dialogTitle,
  getTooltipDescription,
}: ExportModalProps) => {
  return (
    <Dialog
      hideBackdrop
      PaperProps={{ sx: { minWidth: "398px" } }}
      open={dialogOpen}
    >
      <DialogTitle>{dialogTitle}</DialogTitle>
      {ContentComponent}
      <ExportModalActions
        handleExportSelected={handleExportSelected}
        exportDisabled={exportDisabled}
        pauseDisabled={pauseDisabled}
        onCancel={onCancel}
        getTooltipDescription={getTooltipDescription}
      />
    </Dialog>
  );
};

default ExportModal;

} // namespace elizaos
