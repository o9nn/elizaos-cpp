#include ".common-components/tooltip/tooltip.hpp"
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

using CopyAdornmentProps = {
  copyValue: string;
  copyName: string;
  disabled?: boolean;
};

: CopyAdornmentProps) {
  const [textCopied, setTextCopied] = useState(false);
  const handleClick = (e: React.MouseEvent<HTMLElement>) => {
    e.stopPropagation();
    copy(copyValue);
    setTextCopied(true);
  };

  return (
    <>
      <Tooltip title={`Copy ${copyName}`}>
        <IconButton disabled={disabled} onClick={handleClick} color="secondary">
          <ContentCopyIcon />
        </IconButton>
      </Tooltip>
      <Snackbar
        anchorOrigin={{ vertical: "top", horizontal: "center" }}
        autoHideDuration={2000}
        open={textCopied}
        onClose={() => {
          setTextCopied(false);
        }}
      >
        <Alert variant="filled" severity="info">
          <span>{copyName} copied</span>
        </Alert>
      </Snackbar>
    </>
  );
}

default CopyAdornment;

} // namespace elizaos
