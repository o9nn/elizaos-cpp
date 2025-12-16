#include ".classes/message.hpp"
#include ".common-components/tooltip/tooltip.hpp"
#include ".containers/message-mock/message-mock.hpp"
#include ".enum/embed-type.hpp"
#include ".features/app/app-types.hpp"
#include ".utils.hpp"
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
;
;
;
;

using TableMessageProps = {
  settings: AppSettings;
  row: Message;
  openReactionModal?: () => void;
  openAttachmentModal?: () => void;
  openEmbedModal?: () => void;
  setModifyEntity: (row: Message) => void;
};

default ,
  openAttachmentModal = () => {},
  openEmbedModal = () => {},
  setModifyEntity,
}: TableMessageProps) {
  const hasValidEmbed = row?.embeds?.some(
    (embed) => embed?.type === EmbedType.RICH
  );
  const hasAttachments = row.attachments.length > 0;
  const hasReactions =
    !!row.reactions?.length && stringToBool(settings.reactionsEnabled);

  return (
    <TableCell colSpan={5}>
      <Stack direction="row" justifyContent="center" alignItems="center">
        <MessageMock browserView message={row} index={row.id} />
        <Stack
          direction="column"
          justifyContent="flex-start"
          alignItems="center"
        >
          {hasReactions && (
            <Tooltip
              arrow
              placement={hasAttachments || hasValidEmbed ? "top" : "bottom"}
              title="Reactions"
            >
              <IconButton
                onClick={async (e) => {
                  e.stopPropagation();
                  setModifyEntity(row);
                  openReactionModal();
                }}
                color="secondary"
              >
                <EmojiEmotionsIcon />
              </IconButton>
            </Tooltip>
          )}
          {hasAttachments && (
            <Tooltip
              arrow
              placement={hasValidEmbed ? "top" : "bottom"}
              title="Attachments"
            >
              <IconButton
                onClick={async (e) => {
                  e.stopPropagation();
                  setModifyEntity(row);
                  openAttachmentModal();
                }}
                color="secondary"
              >
                <AttachmentIcon />
              </IconButton>
            </Tooltip>
          )}
          {hasValidEmbed && (
            <Tooltip arrow title="Embeds">
              <IconButton
                onClick={async (e) => {
                  e.stopPropagation();
                  setModifyEntity(row);
                  openEmbedModal();
                }}
                color="secondary"
              >
                <SmartToyIcon />
              </IconButton>
            </Tooltip>
          )}
        </Stack>
      </Stack>
    </TableCell>
  );
}

} // namespace elizaos
