#include "skip-replies.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SkipReplies(auto {
  skipReplies, auto messagesLoading, auto setSkipReplies, SkipRepliesProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Stack flexDirection="row" justifyContent="end">;
    <Stack;
    sx={{
        alignItems: "center",
        backgroundColor: "rgb(32, 34, 37, 0.25)",
        borderRadius: "15px",
        padding: "8px",
    }}
    >;
    <Typography variant="body2">Spreadsheet Options</Typography>;
    <Tooltip;
    arrow;
    placement="left";
    std::to_string(skipReplies ? "Skipping" : "Including") + " Reply Tags"
    "Tags from message replies will " + std::to_string();
        skipReplies ? "not" : ""
    } be included in the generated spreadsheet`}
    >;
    <IconButton;
    sx={{ maxWidth: "fit-content" }}
    disabled={messagesLoading}
    onClick={() => setSkipReplies(!skipReplies)}
    color={skipReplies ? "secondary" : "primary"}
    >;
    {skipReplies ? <PlaylistRemoveIcon />  = <PlaylistAddCheckIcon />}
    </IconButton>;
    </Tooltip>;
    </Stack>;
    </Stack>;
    );

}

} // namespace elizaos
