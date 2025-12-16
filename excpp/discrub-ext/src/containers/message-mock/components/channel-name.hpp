#include "...classes/channel.hpp"
#include "...classes/message.hpp"
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

using ChannelNameProps = std::vector<{
  channels: Channel>;
  message: Message;
  thread: Channel | undefined;
};

const ChannelName = ({ channels, message, thread }: ChannelNameProps) => {
  const theme = useTheme();

  return (
    <Typography
      variant="caption"
      mt="1px"
      sx={{
        textDecoration: "underline",
        textDecorationStyle: "dotted",
        color: theme.palette.text.primary,
      }}
    >
      {
        channels.find(
          (channel) =>
            channel.id === message.channel_id ||
            thread?.parent_id === channel.id
        )?.name
      }
    </Typography>
  );
};
default ChannelName;

} // namespace elizaos
