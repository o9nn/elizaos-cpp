#include "...classes/guild.hpp"
#include "...classes/message.hpp"
#include "...components/author-avatar.hpp"
#include "...features/export/export-types.hpp"
#include "...utils.hpp"
#include "author-name.hpp"
#include "message-content.hpp"
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
;
;
;

using RepliedToContentProps = {
  browserView: boolean;
  message: Message;
  userMap: ExportUserMap;
  selectedGuild: Guild | Maybe;
  id: string;
  rawHtml: string;
  getRolePath: (x: Snowflake, y: string | Maybe) => ResolvedFilePathObject;
};

const RepliedToContent = ({
  browserView,
  message,
  userMap,
  selectedGuild,
  id,
  rawHtml,
  getRolePath,
}: RepliedToContentProps) => {
  return (
    <Stack
      direction="row"
      alignItems="flex-start"
      justifyContent="flex-start"
      spacing={1}
    >
      <div
        style={{
          marginLeft: 20,
          marginTop: 8,
          height: 7,
          width: 17,
          borderLeft: "2px solid #4e5058",
          borderTop: "2px solid #4e5058",
          borderTopLeftRadius: "5px",
        }}
      />
      <Stack
        direction="row"
        alignItems="center"
        justifyContent="flex-start"
        spacing={1}
        sx={{ maxWidth: 600 }}
      >
        <AuthorAvatar browserView={browserView} message={message} isReply />
        <Typography
          sx={{
            display: "flex",
            gap: "5px",
            "& a": {
              display: "flex",
              gap: "5px",
              color: "inherit",
              textDecoration: "inherit",
            },
            "& span": {
              color: "#FFF !important",
            },
            color: "#a0a1a4",
            whiteSpace: "nowrap",
          }}
          variant="caption"
        >
          {browserView ? (
            <AuthorName
              msg={message}
              getRolePath={getRolePath}
              userMap={userMap}
              selectedGuild={selectedGuild}
            />
          ) : (
            <a href={`#${message.id}`}>
              <AuthorName
                msg={message}
                getRolePath={getRolePath}
                userMap={userMap}
                selectedGuild={selectedGuild}
              />
            </a>
          )}
        </Typography>
        <MessageContent id={id} isReply={true} rawHtml={rawHtml} />
      </Stack>
    </Stack>
  );
};
default RepliedToContent;

} // namespace elizaos
