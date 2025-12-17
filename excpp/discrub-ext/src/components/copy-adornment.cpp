#include "copy-adornment.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void CopyAdornment(auto {
  copyValue, auto copyName, auto disabled = false, CopyAdornmentProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [textCopied, setTextCopied] = useState(false);
    const auto handleClick = [&](e: React.MouseEvent<HTMLElement>) {;
        e.stopPropagation();
        copy(copyValue);
        setTextCopied(true);
        };

        return (;
        <>;
        "Copy " + std::to_string(copyName);
        <IconButton disabled={disabled} onClick={handleClick} color="secondary">;
        <ContentCopyIcon />;
        </IconButton>;
        </Tooltip>;
        <Snackbar;
    anchorOrigin={{ vertical: "top", horizontal: "center" }}
    autoHideDuration={2000}
    open={textCopied}
    onClose={() => {
        setTextCopied(false);
    }}
    >;
    <Alert variant="filled" severity="info">;
    <span>{copyName} copied</span>;
    </Alert>;
    </Snackbar>;
    </>;
    );

}

} // namespace elizaos
