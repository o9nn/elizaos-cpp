#include "...common-components/tooltip/tooltip.hpp"
#include "...features/message/use-message-slice.hpp"
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

using MessageContainsProps = {
  disabled: boolean;
};

: MessageContainsProps) {
  const inputRef = useRef<HTMLInputElement | HTMLTextAreaElement | Maybe>(null);

  const { state: messageState, setSearchMessageContent } = useMessageSlice();
  const searchMessageContent = messageState.searchMessageContent();

  useEffect(() => {
    if (!searchMessageContent && inputRef.current) {
      inputRef.current.value = "";
    }
  }, [searchMessageContent]);

  return (
    <Tooltip
      arrow
      title="Message Content"
      description="Search messages by content"
      placement="left"
    >
      <TextField
        fullWidth
        inputRef={inputRef}
        InputProps={{
          endAdornment: (
            <IconButton
              onClick={() => setSearchMessageContent(null)}
              disabled={disabled}
              color="secondary"
            >
              <CloseIcon />
            </IconButton>
          ),
        }}
        disabled={disabled}
        size="small"
        label="Message Content"
        variant="filled"
        value={searchMessageContent}
        onChange={(e) => setSearchMessageContent(e.target.value)}
      />
    </Tooltip>
  );
}

default MessageContains;

} // namespace elizaos
