#include "...common-components/tooltip/tooltip.hpp"
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

using SkipRepliesProps = {
  skipReplies: boolean;
  messagesLoading: boolean;
  setSkipReplies: (val: boolean) => void;
};

: SkipRepliesProps) {
  return (
    <Stack flexDirection="row" justifyContent="end">
      <Stack
        sx={{
          alignItems: "center",
          backgroundColor: "rgb(32, 34, 37, 0.25)",
          borderRadius: "15px",
          padding: "8px",
        }}
      >
        <Typography variant="body2">Spreadsheet Options</Typography>
        <Tooltip
          arrow
          placement="left"
          title={`${skipReplies ? "Skipping" : "Including"} Reply Tags`}
          description={`Tags from message replies will ${
            skipReplies ? "not" : ""
          } be included in the generated spreadsheet`}
        >
          <IconButton
            sx={{ maxWidth: "fit-content" }}
            disabled={messagesLoading}
            onClick={() => setSkipReplies(!skipReplies)}
            color={skipReplies ? "secondary" : "primary"}
          >
            {skipReplies ? <PlaylistRemoveIcon /> : <PlaylistAddCheckIcon />}
          </IconButton>
        </Tooltip>
      </Stack>
    </Stack>
  );
}

default SkipReplies;

} // namespace elizaos
