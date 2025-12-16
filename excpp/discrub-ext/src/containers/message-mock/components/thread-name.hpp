#include "...classes/channel.hpp"
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

using ThreadNameProps = { thread: Channel };

const ThreadName = ({ thread }: ThreadNameProps) => {
  const theme = useTheme();

  return (
    <Typography variant="caption" sx={{ color: theme.palette.text.disabled }}>
      {thread.name}
    </Typography>
  );
};
default ThreadName;

} // namespace elizaos
