#include "...common-components/tooltip/tooltip.hpp"
#include "...components/cancel-button.hpp"
#include "...components/pause-button.hpp"
#include "...enum/export-type.hpp"
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

using ActionsProps = {
  handleExportSelected: (val: ExportType) => void;
  onCancel: () => void;
  exportDisabled: boolean;
  pauseDisabled: boolean;
  getTooltipDescription: (exportType: ExportType) => string;
};

const ExportModalActions = ({
  handleExportSelected,
  onCancel,
  exportDisabled,
  pauseDisabled,
  getTooltipDescription,
}: ActionsProps) => {
  const [anchorEl, setAnchorEl] = useState<HTMLButtonElement | null>(null);
  const open = !!anchorEl;
  const handleClick = (event: React.MouseEvent<HTMLButtonElement>) => {
    setAnchorEl(event.currentTarget);
  };
  const handleClose = () => {
    setAnchorEl(null);
  };

  const handleExport = (val: ExportType) => {
    handleClose();
    handleExportSelected(val);
  };

  const tooltipTitle = "Export Includes:";
  const tooltipPlacement = "left";

  return (
    <DialogActions>
      <CancelButton onCancel={onCancel} />
      <PauseButton disabled={pauseDisabled} />
      <Button
        disabled={exportDisabled}
        variant="contained"
        disableElevation
        onClick={handleClick}
        endIcon={<KeyboardArrowDownIcon />}
      >
        Export
      </Button>

      <Menu anchorEl={anchorEl} open={open} onClose={handleClose}>
        {Object.values(ExportType).map((exportType) => (
          <Tooltip
            placement={tooltipPlacement}
            title={tooltipTitle}
            description={getTooltipDescription(exportType)}
          >
            <MenuItem dense onClick={() => handleExport(exportType)}>
              {exportType === ExportType.MEDIA
                ? "Media Only"
                : exportType.toUpperCase()}
            </MenuItem>
          </Tooltip>
        ))}
      </Menu>
    </DialogActions>
  );
};

default ExportModalActions;

} // namespace elizaos
