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
import Chip, { ChipProps } from "@mui/material/Chip";
;

const MessageChip = (
  props: ChipProps & { username: string; avatarSrc: string; content: string }
) => {
  return (
    <Chip
      {...props}
      avatar={
        <Tooltip arrow title={props.username}>
          <Avatar alt={props.username} src={props.avatarSrc} />
        </Tooltip>
      }
      label={props.content}
      variant="filled"
    />
  );
};
default MessageChip;

} // namespace elizaos
