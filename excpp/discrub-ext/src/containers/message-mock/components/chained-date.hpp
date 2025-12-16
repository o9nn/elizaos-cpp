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

using ChainedDateProps = { message: Message; longDateTime: string };

const ChainedDate = ({ message, longDateTime }: ChainedDateProps) => {
  const theme = useTheme();
  const messageDate = parseISO(message.timestamp);
  const shortTime = format(messageDate, "HH:mm:ss");

  return (
    <Box
      title={longDateTime}
      sx={{
        width: "fit-content",
        height: "20px",
        display: "flex",
        alignItems: "center",
      }}
    >
      <Typography
        id={`chained-message-${message.id}`}
        sx={{
          fontSize: "10px !important",
          color: theme.palette.text.disabled,
          opacity: 0,
        }}
        variant="caption"
      >
        {shortTime}
      </Typography>
    </Box>
  );
};
default ChainedDate;

} // namespace elizaos
