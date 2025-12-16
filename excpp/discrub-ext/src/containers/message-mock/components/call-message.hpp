#include "...classes/user.hpp"
#include "...types/message-call-object.hpp"
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
import AuthorName, { AuthorNameProps } from "./author-name";
;
;
import Date, { DateProps } from "./date";

using CallMessageProps = std::variant<{
  currentUser: User, Maybe>;
  call: MessageCallObject | Maybe;
} & AuthorNameProps &
  DateProps;

const CallMessage = ({
  currentUser,
  call,
  msg,
  userMap,
  selectedGuild,
  longDateTime,
  shortDateTime,
  getRolePath,
}: CallMessageProps) => {
  const theme = useTheme();
  const { timestamp } = msg;
  const { participants, ended_timestamp } = call || { participants: [] };
  const { id } = currentUser || { id: null };
  const answered = participants.some((pId) => pId === id);
  let duration = null;
  if (timestamp && ended_timestamp) {
    const rawDuration = intervalToDuration({
      start: parseISO(timestamp),
      end: parseISO(ended_timestamp),
    });
    duration = formatDuration(rawDuration, {
      format: ["hours", "minutes", "seconds"],
    });
  }
  return (
    <Stack
      sx={{
        flexDirection: "row",
        alignItems: "center",
        gap: "5px",
        flexWrap: "wrap",
      }}
    >
      <PhoneIcon
        sx={{
          color: answered
            ? theme.palette.success.main
            : theme.palette.text.disabled,
        }}
      />
      <Typography style={{ color: theme.palette.text.disabled }}>
        {answered ? "" : "You missed a call from "}
      </Typography>
      <Typography sx={{ color: theme.palette.text.primary }}>
        <AuthorName
          msg={msg}
          userMap={userMap}
          selectedGuild={selectedGuild}
          getRolePath={getRolePath}
        />
      </Typography>
      <Typography style={{ color: theme.palette.text.disabled }}>
        {answered
          ? ` started a call that lasted ${duration}.`
          : ` that lasted ${duration}.`}
      </Typography>
      <Date longDateTime={longDateTime} shortDateTime={shortDateTime} />
    </Stack>
  );
};
default CallMessage;

} // namespace elizaos
