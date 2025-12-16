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
import AuthorName, { AuthorNameProps } from "./author-name";
;
import Date, { DateProps } from "./date";

using PinMessageProps = AuthorNameProps & DateProps;

const PinMessage = ({
  msg,
  userMap,
  selectedGuild,
  longDateTime,
  shortDateTime,
  getRolePath,
}: PinMessageProps) => {
  const theme = useTheme();

  return (
    <Stack
      sx={{
        flexDirection: "row",
        alignItems: "center",
        gap: "5px",
        flexWrap: "wrap",
      }}
    >
      <PushPinIcon
        sx={{
          color: theme.palette.text.disabled,
        }}
      />
      <Typography sx={{ color: theme.palette.text.primary }}>
        <AuthorName
          msg={msg}
          userMap={userMap}
          selectedGuild={selectedGuild}
          getRolePath={getRolePath}
        />
      </Typography>
      <Typography style={{ color: theme.palette.text.disabled }}>
        {` pinned a message to this channel.`}
      </Typography>
      <Date longDateTime={longDateTime} shortDateTime={shortDateTime} />
    </Stack>
  );
};
default PinMessage;

} // namespace elizaos
