#include "message-contains.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MessageContains() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto inputRef = useRef<HTMLInputElement | HTMLTextAreaElement | Maybe>(nullptr);

    const auto { state: messageState, setSearchMessageContent } = useMessageSlice();
    const auto searchMessageContent = messageState.searchMessageContent();

    useEffect(() => {
        if (!searchMessageContent && inputRef.current) {
            inputRef.current.value = "";
        }
        }, [searchMessageContent]);

        return (;
        <Tooltip;
        arrow;
        title="Message Content";
        description="Search messages by content";
        placement="left";
        >;
        <TextField;
        fullWidth;
    inputRef={inputRef}
    InputProps={{
        endAdornment: (
        <IconButton;
    onClick={() => setSearchMessageContent(nullptr)}
    disabled={disabled}
    color="secondary";
    >;
    <CloseIcon />;
    </IconButton>;
    ),
    }}
    disabled={disabled}
    size="small";
    label="Message Content";
    variant="filled";
    value={searchMessageContent}
    onChange={(e) => setSearchMessageContent(e.target.value)}
    />;
    </Tooltip>;
    );

}

} // namespace elizaos
