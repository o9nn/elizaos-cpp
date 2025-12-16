#include "...common-components/tooltip/tooltip.hpp"
#include "...services/chrome-service.hpp"
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

const CloseWindowButton = () => {
  return (
    <Box sx={{ position: "fixed", top: "12px", right: "267px" }}>
      <Tooltip placement="left" arrow title="Quit">
        <IconButton
          onClick={() => sendChromeMessage("CLOSE_INJECTED_DIALOG")}
          color="secondary"
        >
          <CloseOutlinedIcon />
        </IconButton>
      </Tooltip>
    </Box>
  );
};

default CloseWindowButton;

} // namespace elizaos
